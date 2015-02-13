RM=rm -f
CXX=g++
CXXFLAGS=-std=c++11 -g `pkg-config dbus-c++-1 --cflags`
LDLIBS=`pkg-config dbus-c++-1 --libs`

all: fclip fclip-server

fclip-server: server.o FclipServer.o Clipboard.o TreeNode.o file_functions.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lboost_filesystem -lboost_system

server.o: server.cpp FclipServer.h Fclip_adaptor.h

FclipServer.o: FclipServer.cpp FclipServer.h Clipboard.h Fclip_adaptor.h

Clipboard.o: Clipboard.cpp Clipboard.h TreeNode.h file_functions.h

TreeNode.o: TreeNode.cpp TreeNode.h

Fclip_adaptor.h: dbus-service.xml
	dbusxx-xml2cpp $< --adaptor=$@


fclip: client.o file_functions.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lboost_program_options -lboost_system -lboost_filesystem

client.o: client.cpp FclipClient.h Fclip_proxy.h

Fclip_proxy.h: dbus-service.xml
	dbusxx-xml2cpp $< --proxy=$@


file_functions.o: file_functions.cpp file_functions.h

clean:
	$(RM) *.o fclip fclip-server