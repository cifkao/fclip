#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "FclipServer.h"

using namespace std;

void FclipServer::ListFilesToStream(const string& directory,
  const bool& absolute,
  const string& stream,
  vector< string >& messages,
  bool& success) {

  ofstream ofstr;
  ofstr.open(stream);

  auto fn = [&ofstr](string filename, bool recursive){
    ofstr << recursive << " " << filename << endl;
  };

  success = clip_.forEachFile(directory, absolute, fn, messages);

  ofstr.close();
}
