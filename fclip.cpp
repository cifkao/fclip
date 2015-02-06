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


bool oVerbose;
bool oNonRecursive;

DBus::BusDispatcher dispatcher;

vector<string> serverMessages;


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
  return po::options_description();
}

po::options_description list_options(){
  return po::options_description();
}

bool help_run(const vector<string> &argv){
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
    const string &cmd = vm["command"].as<string>();
    if(cmd == "help"){
      cout << "usage: fclip help [<command>]" << endl << endl;
      cout << "Displays help information about a command." << endl;
      cout << help_options() << endl;
    }else if(cmd == "add"){
      cout << "usage: fclip add [<options>] <files>..." << endl << endl;
      cout << "Adds a file to the clipboard." << endl << endl;
      cout << add_options() << endl;
    }
    return true;
  }
  return false;
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
    fclip.Remove(files, serverMessages, success);
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

string expandCommand(string cmd){
  if(cmd == "rm") return "remove";
  if(cmd == "ls") return "list";
  if(cmd == "ls-all") return "list-all";
  return cmd;
}

int main(int argc, char** argv) {
  po::options_description options("Options");
  options.add_options()
          ("help,h", "display help and exit")
          ("verbose,v", po::bool_switch(&oVerbose), "explain what is being done");
  
  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argc, argv)
            .options(options)
            .allow_unregistered()
            .run();
  po::store(parsed, vm);
  po::notify(vm);
  
  vector<string> subargs = po::collect_unrecognized(parsed.options, po::include_positional);
  
  if(vm.count("help") || subargs.empty()) {
    if(subargs.empty()){
      cout << options << endl;
      return EXIT_FAILURE;
    }else{
      subargs.insert(subargs.begin(), "help");
    }
  }
  
  string cmd = expandCommand(subargs[0]);
  subargs.erase(subargs.begin());
  
  if(cmd == "help"){
    if(!help_run(subargs))
      cout << options << endl;
    return EXIT_FAILURE;
  }
  
  bool success = true;
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
    }else{
      throw runtime_error("unrecognised command or option '" + cmd + "'");
    }
    
    for(const string &str : serverMessages){
      cerr << str << endl;
    }
  }catch(const DBus::Error &e){
    cerr << "failed to connect to fclip server" << endl;
    return EXIT_FAILURE;
  }catch(const exception &e){
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
