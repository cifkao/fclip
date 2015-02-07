#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
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
