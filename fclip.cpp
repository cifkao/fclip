#include <vector>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>
#include <dbus-c++/dbus.h>
#include "file_functions.h"

#include "FclipClient.h"

using namespace std;
namespace po = boost::program_options;


// global options:
bool oVerbose;
// "add" options:
bool oNonRecursive;
// "remove" options:
bool oRemoveParents;
// "for-each" options:
bool oForEachRemove;


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
  return po::options_description();
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
      cout << "usage: fclip for-each <commands>..." << endl;
      cout << "aliases: foreach, each" << endl << endl;
      cout << "Perform actions for each file in the clipboard." << endl << endl;
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
    namespace fs = boost::filesystem;
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
  }else return false;
}

bool forEach_run(const vector<string> &argv, FclipClient &fclip){
  po::options_description options = list_options();
  options.add_options()
          ("command", po::value<vector<string> >());
  po::positional_options_description positional;
  positional.add("command", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argv)
            .options(options)
            .positional(positional)
            .run(), vm);
  po::notify(vm);
  
  string fifoPath;
  {
    namespace fs = boost::filesystem;
    fs::path fifoPath_ = fs::temp_directory_path() / fs::unique_path();
    fifoPath = fifoPath_.string();
  }
  
  
  
  if(vm.count("command")){
    vector<string> commands = vm["command"].as<vector<string> >();
    for(auto cmd : commands){
      system(cmd.c_str());
    }
  }
  
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
