#include "Server.hpp"
#include "interface.hpp"
#include "util.hpp"
#include <iostream>
#include <thread>

using namespace std;

int main() {
  // Create an instance of the Server class
  Server *server = new Server("localhost");

  // Create an instance of the GUI class
  GUI *serverUI = GUI::GetInstance("<Server> ");

  // Initialize the GUI
  serverUI->init();

  // Initialize the server
  server->init();

  // Create a thread to run the server
  std::thread *serverThread = new std::thread([server, serverUI]() {
    while (server->isRunning())
      ;
  });

  // Run the GUI
  serverUI->run();

  // Set the flag to stop the server
  server->shouldBeRunning = false;

  // Wait for the server thread to finish
  serverThread->join();

  // Stop the server
  server->stop();

  // Close the GUI
  serverUI->close();

  return 0;
}