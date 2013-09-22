#tagfs: fused.o server.o fused.h server.h
tagfs: server.cpp
	g++ server.cpp -o tagfs -I../lib/ext/libmicrohttpd/src/include -I../lib/ext/leplib/src -L../lib/ext/libmicrohttpd/src/microhttpd/.libs -L../lib/ext/leplib/build/src -lmicrohttpd -lleplib
