#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

// Default port for the client
#define DEFAULT_PORT "6697"

// Maximum size of a message
#define MAX_MSG_SIZE 4096

#include "Socket.hpp" // Include the Socket header file
#include <mutex>
#include <string>
#include <thread>

// Class representing a client
class Client {
private:
  MySocket *socket;            // Pointer to a MySocket object
  std::string address;         // Address of the server
  SocketWithInfo *clientInfo;  // Pointer to a SocketWithInfo object
  bool _isConnected = false;   // Flag indicating if the client is connected
  std::mutex isConnectedMutex; // Mutex for thread safety
  bool shouldBeListening =
      false;      // Flag indicating if the client should be listening
  void _listen(); // Private method for listening to incoming messages
  std::thread *listenThread; // Pointer to a thread for listening
  void init();               // Private method for initializing the client

public:
  // Constructor with address parameter
  Client(std::string address);

  // Default constructor
  Client();

  // Method to start the client with the specified address
  int start(std::string address);

  // Method to start the client with the default address
  int start();

  // Method to start listening for incoming messages
  void startListening();

  // Method to stop the client
  int stop();

  // Method to check if the client is connected
  bool isConnected(bool);

  // Method to read a message from the server
  std::string readMessage();

  // Method to safely read a message from the server
  int safeReadMessage(std::string &message);

  // Method to send a message to the server
  void sendMessage(std::string message);

  // Method to send a message to the server and receive a response
  void messageServer(std::string message);

  // Method to check if the client has a channel
  bool hasChannel(bool);

  // Method to check if the client is muted
  bool checkMute();
};

#endif