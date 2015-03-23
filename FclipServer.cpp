#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include "Clipboard.h"
#include "FclipServer.h"

using namespace std;

void FclipServer::ListFilesToStream(const string& directory,
  const bool& absolute,
  const string& stream) {

  ofstream ofstr;
  ofstr.open(stream);

  auto fn = [&ofstr](string filename, bool recursive){
    ofstr << recursive << " " << filename.length() << "\n";
    ofstr << filename << "\n";
  };

  vector<string> messages;
  clip_.forEachFile(directory, absolute, fn, messages);

  ofstr.close();
}

void FclipServer::DirectoryListing(const std::string& directory,
  std::vector< ::DBus::Struct< std::string, bool, bool > >& dbusFiles,
  bool& recursive,
  std::vector< std::string >& messages,
  bool& success) {
  
  Clipboard::directoryListing_t files;
  success = clip_.directoryListing(directory, files, recursive, messages);
  
  for(auto const &f : files){
    ::DBus::Struct<string,bool,bool> fstruct;
    tie(fstruct._1, fstruct._2, fstruct._3) = f;
    dbusFiles.push_back(fstruct);
  }
}
