#include <dbus-c++/dbus.h>

#include "FclipServer.h"

DBus::BusDispatcher dispatcher;

int main(int argc, char** argv) {
  DBus::default_dispatcher = &dispatcher;
	DBus::Connection bus = DBus::Connection::SessionBus();
	bus.request_name("com.fclip.Fclip");
	FclipServer fclip(bus);
	dispatcher.enter();
  
  return EXIT_SUCCESS;
}
