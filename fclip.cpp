#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <cstdio>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dbus-c++/dbus.h>
#include "file_functions.h"

#include "FclipClient.h"

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

// global options:
bool oVerbose;
// "add" options:
bool oNonRecursive;
// "remove" options:
bool oRemoveParents;
// "for-each" options:
bool oForEachRemove;
bool oForEachPrint;


DBus::BusDispatcher dispatcher;

vector<string> serverMessages;


string expandCommand(string cmd){
  if(cmd == "rm") return "remove";
  if(cmd == "ls") return "list";
  if(cmd == "each" || cmd == "foreach") return "for-each";
  return cmd;
}

po::options_description general_options(){
  po::options_description options("Options");
  options.add_options()
          ("help,h", "display help and exit")
          ("verbose,v", po::bool_switch(&oVerbose), "explain what is being done");
  return options;
}

po::options_description help_options(){
  return po::options_description();
}

po::options_description add_options(){
  po::options_description options("Options");
  options.add_options()
          (",n", po::bool_switch(&oNonRecursive), "do not add directory contents recursively");
  return move(options);
}

po::options_description remove_options(){
  po::options_description options("Options");
  options.add_options()
          ("parents,p", po::bool_switch(&oRemoveParents), "remove parent directories if empty");
  return move(options);
}

po::options_description list_options(){
  return po::options_description();
}

po::options_description forEach_options(){
  po::options_description options("Options");
  options.add_options()
          ("exec,c", po::value<vector<string> >(), "execute the given command for each file")
          ("print,p", po::bool_switch(&oForEachPrint), "write all paths to output");
  return move(options);
}

void help_run(const vector<string> &argv){
  po::options_description options = help_options();
  options.add_options()
          ("command", po::value<string>());
  po::positional_options_description positional;
  positional.add("command", 1);
  po::variables_map vm;
  po::store(po::command_line_parser(argv)
            .options(options)
            .positional(positional)
            .allow_unregistered()
            .run(), vm);
  po::notify(vm);
  
  if(vm.count("command")){
    const string &cmd = expandCommand(vm["command"].as<string>());
    if(cmd == "help"){
      cout << "usage: fclip help [<command>]" << endl << endl;
      cout << "Display help information about a command." << endl;
      cout << help_options() << endl;
    }else if(cmd == "add"){
      cout << "usage: fclip add [<options>] <files>..." << endl << endl;
      cout << "Add one or more files to the clipboard." << endl << endl;
      cout << add_options() << endl;
    }else if(cmd == "remove"){
      cout << "usage: fclip remove [<options>] <files>..." << endl;
      cout << "alias: rm" << endl << endl;
      cout << "Remove one or more files from the clipboard." << endl << endl;
      cout << remove_options() << endl;
    }else if(cmd == "for-each"){
      cout << "usage: fclip for-each [<base-dir>] [<options>]" << endl;
      cout << "aliases: foreach, each" << endl << endl;
      cout << setw(80) <<
              "Perform actions for each file in the clipboard.\n"
              "If a base-dir is specified, only files in that directory\n"
              "will be processed." << endl << endl;
      cout << forEach_options() << endl;
    }else goto generalHelp;
    return;
  }
  
  generalHelp:
  cout << "usage: fclip [<options>] <command> [<command-options>]" << endl << endl;
  cout << "Commands:" << endl;
  cout << left;
  cout << setw(24) << "  add " << "add files to the clipboard" << endl;
  cout << setw(24) << "  foreach, each " << "perform actions for each file in the clipboard" << endl;
  cout << setw(24) << "  help " << "get help about a command" << endl;
  cout << setw(24) << "  remove, rm " << "remove files from the clipboard" << endl;
  cout << endl;
  cout << general_options() << endl;
}


bool add_run(const vector<string> &argv, FclipClient &fclip){
  po::options_description options = add_options();
  options.add_options()
          ("file", po::value<vector<string> >());
  po::positional_options_description positional;
  positional.add("file", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argv)
            .options(options)
            .positional(positional)
            .run(), vm);
  po::notify(vm);
  
  if(vm.count("file")){
    vector<string> files = vm["file"].as<vector<string> >();
    // make file paths canonical
    for(auto it = files.begin(); it != files.end(); ++it){
      boost::system::error_code ec;
      string path = file_functions::getCanonicalPathToSymlink(*it, ec).string();
      if(ec.value() != boost::system::errc::success){
        cerr << "Cannot access " << *it << ": " << ec.message() << endl;
        it = files.erase(it);
        if(it == files.end()) break;
      }else{
        *it = move(path);
        if(oVerbose)
          cout << *it << endl;
      }
    }
    
    bool success;
    fclip.Add(files, !oNonRecursive, serverMessages, success);
    return success;
  }else{
    throw runtime_error("no files specified");
  }
}

bool remove_run(const vector<string> &argv, FclipClient &fclip){
  po::options_description options = remove_options();
  options.add_options()
          ("file", po::value<vector<string> >());
  po::positional_options_description positional;
  positional.add("file", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argv)
            .options(options)
            .positional(positional)
            .run(), vm);
  po::notify(vm);
  
  if(vm.count("file")){
    vector<string> files = vm["file"].as<vector<string> >();
    // make file paths canonical
    for(auto it = files.begin(); it != files.end(); ++it){
      boost::system::error_code ec;
      string path = file_functions::getCanonicalPathToSymlink(*it, ec).string();
      if(ec.value() != boost::system::errc::success){
        cerr << "Cannot access " << *it << ": " << ec.message() << endl;
        it = files.erase(it);
        if(it == files.end()) break;
      }else{
        *it = move(path);
        if(oVerbose)
          cout << *it << endl;
      }
    }
    
    bool success;
    fclip.Remove(files, oRemoveParents, serverMessages, success);
    return success;
  }else{
    throw runtime_error("no files specified");
  }
}

bool list_run(const vector<string> &argv, FclipClient &fclip){
  po::options_description options = list_options();
  options.add_options()
          ("path", po::value<string>());
  po::positional_options_description positional;
  positional.add("path", 1);
  po::variables_map vm;
  po::store(po::command_line_parser(argv)
            .options(options)
            .positional(positional)
            .run(), vm);
  po::notify(vm);
  
  string path = ".";
  if(vm.count("path")){
    path = vm["path"].as<string>();
  }
  
  {
    boost::system::error_code ec;
    fs::path path_ = fs::canonical(path, ec);
    if(ec.value() != boost::system::errc::success){
      throw runtime_error("Cannot access " + path + ": " + ec.message());
    }
    path = path_.string();
  }
  
  bool success;
  vector<string> files;
  fclip.DirectoryListing(path, files, serverMessages, success);
  if(success){
    for(auto const &f : files){
      cout << f << endl;
    }
    return true;
  }else return false;
}

typedef pair<string,bool> file;

void forEachDoActions(const fs::path &path, const string &baseDir,
        const vector<string> &commands){
  if(oForEachPrint)
    cout << path.string() << endl;

  for(string cmd : commands){
    // replace {} with path
    {
      string braces("{}");
      size_t pos = 0;
      while((pos = cmd.find(braces, pos)) != string::npos){
        cmd.replace(pos, braces.length(), path.string());
      }
    }

    // execute command
    {
      int workDirFd;
      if(baseDir != ""){
        workDirFd = open(".", O_RDONLY);
        if(chdir(baseDir.c_str()) != 0){
          cerr << "cannot chdir to " << baseDir << endl;
          return;
        }
      }
      
      FILE *output = popen(cmd.c_str(), "r");
      if(output){
        char buf[512]; size_t count;
        while(count = fread(buf, 1, sizeof(buf), output)){
          cout.write(buf, count);
        }
        pclose(output);
      }else{
        cerr << "error executing " << cmd << endl;
      }
      
      if(baseDir != ""){
        fchdir(workDirFd);
      }
    }
  }
}

void forEachWorker(const string &baseDir, const vector<string> &commands,
        queue<file> &qu, mutex &qLock, const bool &done, condition_variable &cv){
  while(true){
    fs::path path;
    bool recursive;
    {
      unique_lock<mutex> lk(qLock);
      if(qu.empty()){
        if(done) break;
        cv.wait(lk);
        continue;
      }

      path = qu.front().first;
      recursive = qu.front().second;
      qu.pop();
    }
    
    // now do the actual work
    
    // get file type
    boost::system::error_code ec;
    fs::file_status fstatus = fs::symlink_status(baseDir / path, ec);
    if(ec.value() != boost::system::errc::success){
      cerr << "cannot access " << path.string() << ": " << ec.message() << endl;
      continue;
    }
    
    forEachDoActions(path, baseDir, commands);
    
    // iterate over directory contents recursively
    if(recursive && fs::is_directory(fstatus)){
      try{
        fs::recursive_directory_iterator it(baseDir / path);
        fs::recursive_directory_iterator end;

        while(it != end){
          fs::path path = file_functions::removePathPrefix((*it).path(), baseDir);

          forEachDoActions(path, baseDir, commands);

          // don't follow symlinks
          if(fs::is_directory(*it) && fs::is_symlink(*it))
            it.no_push();

          try{ ++it; }
          catch(exception& ex){
            cerr << "cannot access contents of " << (*it).path().string() << endl;
            it.no_push();
            try{ ++it; }
            catch(exception& ex2){
              cerr << ex2.what() << endl;
              it.pop();
            }
          }
        }
      }catch(exception& ex){
        cerr << "cannot access contents of " << baseDir / path << endl;
      }
    }
  }
}

bool forEach_run(const vector<string> &argv, FclipClient &fclip){
  po::options_description options = forEach_options();
  options.add_options()
          ("base-dir", po::value<string>());
  po::positional_options_description positional;
  positional.add("base-dir", 1);
    
  po::variables_map vm;
  po::store(po::command_line_parser(argv)
            .options(options)
            .positional(positional)
            .run(), vm);
  po::notify(vm);
  
  vector<string> commands;
  if(vm.count("exec"))
    commands = vm["exec"].as<vector<string> >();
  
  string baseDir = "";
  if(vm.count("base-dir")){
    boost::system::error_code ec;
    baseDir = vm["base-dir"].as<string>();
    string baseDir_ = fs::canonical(baseDir, ec).string();
    if(ec.value() != boost::system::errc::success){
      throw runtime_error("Cannot access " + baseDir + ": " + ec.message());
    }
    baseDir = baseDir_;
  }
  
  string pipePath;
  {
    fs::path pipePath_ = fs::temp_directory_path() / fs::unique_path();
    pipePath = pipePath_.string();
  }
  
  if(mkfifo(pipePath.c_str(), S_IFIFO | 0666) != 0)
    throw runtime_error("cannot create named pipe");
  
  fclip.ListFilesToStream(baseDir, false, pipePath);
  
  queue<file> qu;
  mutex qLock;
  bool done = false;
  condition_variable cv;
  thread worker(forEachWorker, ref(baseDir), ref(commands), ref(qu), ref(qLock), ref(done), ref(cv));
  
  try{
    ifstream pipe(pipePath, ios_base::in);

    // read paths from pipe
    string line;
    while (std::getline(pipe, line)){
      std::istringstream iss(line);
      bool recursive; size_t length;
      if(!(iss >> recursive >> length))
        throw runtime_error("invalid data received from server");

      char *c_str = new char[length];
      pipe.read(c_str, length);
      pipe.get(); // read the newline character
      string str(c_str, length);

      if(str.length() != length)
        throw runtime_error("invalid data received from server");
      
      {
        // push path to queue and wake up worker
        unique_lock<mutex> lk(qLock);
        qu.push(make_pair(str, recursive));
        cv.notify_all();
      }
    }
    fs::remove(pipePath);
  }catch(...){
    fs::remove(pipePath);
    throw;
  }
  
  {
    // notify worker that no more paths will be added to the queue
    unique_lock<mutex> lk(qLock);
    done = true;
    cv.notify_all();
  }
  worker.join();
  
  return true;
}

int main(int argc, char** argv) {
  bool success = true;
  
  try{
    po::options_description options = general_options();
    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv)
              .options(options)
              .allow_unregistered()
              .run();
    po::store(parsed, vm);
    po::notify(vm);
  
    vector<string> subargs = po::collect_unrecognized(parsed.options, po::include_positional);

    if(vm.count("help") || subargs.empty()) {
      subargs.insert(subargs.begin(), "help");
      if(subargs.empty()){
        help_run(subargs);
        return EXIT_FAILURE;
      }
    }

    string cmd = expandCommand(subargs[0]);
    subargs.erase(subargs.begin());

    if(cmd == "help"){
      help_run(subargs);
      return EXIT_FAILURE;
    }

    try{
      DBus::default_dispatcher = &dispatcher;
      DBus::Connection bus = DBus::Connection::SessionBus();
      FclipClient fclip(bus);

      if(cmd == "add"){
        success = success && add_run(subargs, fclip);
      }else if(cmd == "remove"){
        success = success && remove_run(subargs, fclip);
      }else if(cmd == "list"){
        success = success && list_run(subargs, fclip);
      }else if(cmd == "for-each"){
        success = success && forEach_run(subargs, fclip);
      }else{
        throw runtime_error("unrecognised command or option '" + cmd + "'");
      }

      for(const string &str : serverMessages){
        cerr << str << endl;
      }
    }catch(const DBus::Error &e){
      cerr << "failed to connect to fclip server" << endl;
      return EXIT_FAILURE;
    }
  }catch(const exception &e){
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
