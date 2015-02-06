#include <dbus-c++/dbus.h>
#include "Fclip_proxy.h"

#ifndef FCLIPCLIENT_H
#define	FCLIPCLIENT_H

class FclipClient : public com::fclip::Fclip_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy {
public:
  FclipClient(DBus::Connection &connection) :
    DBus::ObjectProxy(connection, "/com/fclip/Fclip", "com.fclip.Fclip") {}
};

#endif	/* FCLIPCLIENT_H */

