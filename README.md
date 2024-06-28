# P9-Threads-Boost
## Autor: Adrián Montes Linares

# Objetivo
- Esta práctica consiste en un servidor que reenvia a sus clientes los mensajes que va reciviendo de otros clientes. Es decir, si cliente1 envia un mensaje al servidor, el resto de clientes lo recivirán inlcuyendose a él mismo

## Cliente
```cpp
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <iomanip>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

// Function to read data from a socket until a newline character is encountered
string read_(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until( socket, buf, "\n" );
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

// Function to send a message through a socket, appending a newline character at the end
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(msg) );
}

// Function to continuously receive messages from the server
void receive_messages(tcp::socket & socket) {
    while (true) {
        string reply = read_(socket);
        cout << "\n" << reply << "Enter message: " << std::flush;
    }
}

int main() {
    // Connect to the server
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));

    // Start a new thread to receive messages from the server
    std::thread(receive_messages, std::ref(socket)).detach();

    while (true) {
        // Read a message from the user and send it to the server
        string message;
        cout << "Enter message: ";
        std::getline(std::cin, message);

        send_(socket, message);
    }

    return 0;
}
```

## Servidor
```cpp
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

// Function to read data from a socket until a newline character is encountered
string read_(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until( socket, buf, "\n" );
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

// Function to send a message through a socket, appending a newline character at the end
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(msg) );
}

// Vector to store all active client sockets
std::vector<std::shared_ptr<tcp::socket>> client_sockets;

// Function to handle communication with a client
void client_session(std::shared_ptr<tcp::socket> socket, int id) {
    try {
        while (true) {
            // Read a message from the client
            string message = read_(*socket);
            cout << "Client " << id << ": " << message << endl;
            // Send the message to all clients
            for(auto& client_socket : client_sockets) {
                send_(*client_socket, "Server: " + message);
            }
        }
    } catch (boost::system::system_error& e) {
        // If an error occurs (e.g., the client disconnects), close the socket and remove it from the vector
        cout << "Client " << id << " disconnected." << endl;
        socket->close();
        client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), socket), client_sockets.end());
    }
}

int main() {
    // Set up the server to listen for connections
    boost::asio::io_service io_service;
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234 ));

    int id = 1;
    while (true) {
        // Accept a new client connection
        std::shared_ptr<tcp::socket> socket_(new tcp::socket(io_service));
        acceptor_.accept(*socket_);
        cout << "Client " << id << " connected." << endl;
        // Add the client's socket to the vector
        client_sockets.push_back(socket_);
        // Start a new thread to handle communication with the client
        std::thread(client_session, socket_, id++).detach();
    }

    return 0;
}
```

## Makefile

- Compilar todo:
```bash
make
```

- Borrar:
```bash 
make clean
```