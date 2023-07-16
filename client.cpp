#include "Client.hpp"
#include "interface.hpp"
#include "util.hpp"
#include <iostream>
// #include <readline/history.h>

using namespace std;

int main() {
  // Create a new instance of the Client
  Client *client = new Client();

  // Create a GUI instance and initialize it
  GUI *clientUi = GUI::GetInstance("<Client> ");
  clientUi->init();

  // Add a command to connect to a server

  // Add a command to send a ping message
  clientUi->addCommand("/ping", [client, clientUi](const GUI::argsT &args) {
    if (client->isConnected(true)) {
      client->sendMessage(args[0]);
    }
    return 0;
  });

  // Add a command to change the nickname
  clientUi->addCommand("/nickname", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow("Try: /nickname <nickname>\nHint: This will change "
                            "your current nickname");
      return 1;
    }

    if (client->isConnected(true)) {
      client->sendMessage("/nickname " + args[1]);
    }

    return 0;
  });

  clientUi->addCommand("/connect", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow(
          "Try: /connect <address>\nHint: This will connect you to a Server");
      return 1;
    }
    client->start(args[1]);
    return 0;
  });

  // Add a command to join a channel
  clientUi->addCommand("/join", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow(
          "Try: /join <channel>\nHint: This will connect you to a channel");
      return 1;
    }

    if (client->isConnected(true)) {
      client->sendMessage("/join " + args[1]);
    }

    return 0;
  });

  // Add a command to mute a user
  clientUi->addCommand("/mute", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow("Try: /mute <nickname>\nHint: This will mute a "
                            "user if you are Admin");
      return 1;
    }

    if (client->isConnected(true) && client->hasChannel(true)) {
      client->sendMessage("/mute " + args[1]);
    }

    return 0;
  });

  // Add a command to unmute a user
  clientUi->addCommand("/unmute", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow(
          "Try: /unmute <nickname>\nHint: This will unmute a muted "
          "user if you are Admin");
      return 1;
    }

    if (client->isConnected(true) && client->hasChannel(true)) {
      client->sendMessage("/unmute " + args[1]);
    }

    return 0;
  });

  // Add a command to get the IP address of a user
  clientUi->addCommand("/whois", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow(
          "Try: /whois <nickname>\nHint: This will show selected "
          "user IP address if you are Admin");
      return 1;
    }

    if (client->isConnected(true) && client->hasChannel(true)) {
      client->sendMessage("/whois " + args[1]);
    }

    return 0;
  });

  // Add a command to kick a user
  clientUi->addCommand("/kick", [client, clientUi](const GUI::argsT &args) {
    if (args.size() != 2) {
      // Display error message if command is not used correctly
      clientUi->addToWindow(
          "Try: /kick <nickname>\nHint: This will kick selected "
          "user if you are Admin");
      return 1;
    }

    if (client->isConnected(true) && client->hasChannel(true)) {
      client->sendMessage("/kick " + args[1]);
    }

    return 0;
  });

  // Enable messaging functionality
  clientUi->enableMessaging([client, clientUi](std::string message) {
    if (!client->isConnected(false)) {
      // Display error message if not connected to a server
      GUI::log("You must be connected to send messages!");
      return 0;
    }
    if (!client->hasChannel(false)) {
      // Display error message if not in a channel
      GUI::log("You must be in a channel to send messages!");
      return 0;
    }

    if (client->isMuted()) {
      // Display error message if muted in the channel
      GUI::log("You are muted in this channel!");
      return 0;
    }

    client->messageServer(message);

    return 0;
  });

  // Run the GUI
  clientUi->run();

  // Stop the client
  client->stop();

  // Close the GUI
  clientUi->close();

  return 0;
}