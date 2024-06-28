all: tcp-server tcp-client
	g++ tcp-server.cpp -o tcp-server -lboost_system -pthread
	g++ tcp-client.cpp -o tcp-client -lboost_system -pthread

tcp-server: tcp-server.cpp
	g++ tcp-server.cpp -o tcp-server -lboost_system -pthread

tcp-client: tcp-client.cpp
	g++ tcp-client.cpp -o tcp-client -lboost_system -pthread

clean:
	rm -r -f tcp-server tcp-client