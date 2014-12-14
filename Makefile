RM=rm -f
CXX=g++
CXXFLAGS=-fno-rtti -std=c++11 `pkg-config dbus-c++-1 --cflags`
LDLIBS=`pkg-config dbus-c++-1 --libs` -lboost_filesystem -lboost_system

all: fclipd

fclipd: fclipd.o FclipServer.o Clipboard.o Directory.o File.o file_functions.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

fclipd.o: fclipd.cpp FclipServer.h Fclip_adaptor.h

FclipServer.o: FclipServer.cpp FclipServer.h Clipboard.h Fclip_adaptor.h

Clipboard.o: Clipboard.cpp Clipboard.h Directory.h File.h file_functions.h

Directory.o: Directory.cpp Directory.h TreeNode.h

File.o: File.cpp File.h

file_functions.o: file_functions.cpp file_functions.h

Fclip_adaptor.h: dbus-service.xml
	dbusxx-xml2cpp $< --adaptor=$@

clean:
	$(RM) *.o Fclip_adaptor.h fclipd
