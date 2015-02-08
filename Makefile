RM=rm -f
CXX=g++ -g
CXXFLAGS=-std=c++11 -ggdb `pkg-config dbus-c++-1 --cflags`
LDLIBS=`pkg-config dbus-c++-1 --libs`

all: fclipd fclip

fclipd: fclipd.o FclipServer.o Clipboard.o Directory.o File.o file_functions.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lboost_filesystem -lboost_system

fclipd.o: fclipd.cpp FclipServer.h Fclip_adaptor.h

FclipServer.o: FclipServer.cpp FclipServer.h Clipboard.h Fclip_adaptor.h

Clipboard.o: Clipboard.cpp Clipboard.h Directory.h File.h file_functions.h

Directory.o: Directory.cpp Directory.h TreeNode.h

File.o: File.cpp File.h

Fclip_adaptor.h: dbus-service.xml
	dbusxx-xml2cpp $< --adaptor=$@


fclip: fclip.o file_functions.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lboost_program_options -lboost_system -lboost_filesystem

fclip.o: fclip.cpp FclipClient.h Fclip_proxy.h

Fclip_proxy.h: dbus-service.xml
	dbusxx-xml2cpp $< --proxy=$@


file_functions.o: file_functions.cpp file_functions.h

clean:
	$(RM) *.o fclipd
