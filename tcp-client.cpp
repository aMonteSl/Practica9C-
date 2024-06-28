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