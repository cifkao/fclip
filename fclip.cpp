#include <vector>
#include <string>
#include <iostream>
#include <dbus-c++/dbus.h>

#include "FclipClient.h"

DBus::BusDispatcher dispatcher;

using namespace std;

int main(int argc, char** argv) {
  vector<string> args(argv, argv+argc);
  
  try{
    DBus::default_dispatcher = &dispatcher;
    DBus::Connection bus = DBus::Connection::SessionBus();
    FclipClient fclip(bus);
    
    
  }catch(const DBus::Error &e){
    cerr << "Failed to connect to fclip server" << endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
