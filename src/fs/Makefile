#tagfs: fused.o server.o fused.h server.h
tagfs: server.cpp
	g++ server.cpp -o tagfs -I../lib/ext/libmicrohttpd/src/include -L../lib/ext/libmicrohttpd/src/microhttpd/.libs -lmicrohttpd
