#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#define DEFAULT_PORT "6697"
#define MAX_MSG_SIZE 4096

#include "Socket.hpp"
#include <bits/stdc++.h>
struct Channel {
  std::string channelName;
  std::string admin;
  std::unordered_map<std::string, SocketWithInfo *> users =
      std::unordered_map<std::string, SocketWithInfo *>();
};

class Server {
private:
  MySocket *socket;
  std::string address;
  std::mutex clientsMutex;
  std::unordered_map<std::string, SocketWithInfo *> clients;
  std::unordered_map<std::string, Channel *> channels;
  int nicknameCounter = 1;
  std::string generateDefaultNickname();
  bool checkAvaiableNickname(std::string nickName);
  bool channelExists(std::string channelName);
  bool shouldBeAccepting = false;
  bool shouldBeListening = false;
  std::thread *acceptThread;
  std::thread *listenThread;
  void _accept();
  void _listen();
  void closeClients();
  void closeClient(SocketWithInfo *client);
  SocketWithInfo *clientInfo;
  void handleMessage(SocketWithInfo *client, std::string message);

public:
  Server(std::string address);
  int init();
  int stop();
  bool isRunning();
  bool shouldBeRunning = false;
  std::string readMessage(MySocket *client);
  void sendMessage(std::string message, SocketWithInfo *client);
  void messageClient(std::string message, SocketWithInfo *client,
                     std::string preffix);
  void multicastMessage(std::string message, std::string channel,
                        std::string preffix);
  void acceptClients();
  void listenClients();
};

#endif