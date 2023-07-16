#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <bits/stdc++.h>
#include <netdb.h>

class MySocket;

struct SocketWithInfo {
  std::string nickname;
  MySocket *socket;
  bool isClient;
  bool isAdmin = false;
  bool isMuted = false;
  std::string channel = "";
  SocketWithInfo(MySocket *socket, bool isClient);
};

class MySocket {
private:
  std::string ipAddress;
  std::string portNumber;
  struct addrinfo addressInfo;

public:
  int socketFD;

  MySocket(int domain, int type, int protocol);
  int socketbind(std::string ip, std::string port);
  int socketConnect(std::string ip, std::string port);
  int socketListen(int maxQueue);
  MySocket *accept();
  int socketWrite(std::string msg);
  int socketRead(std::string &buffer, int length);
  int socketSafeRead(std::string &buffer, int length, int timeout);
  int socketSetOpt(int level, int optName, void *optVal);
  int socketGetOpt(int level, int optName, void *optVal);
  int setBlocking(bool blocking);
  int socketShutdown(int how);
  void close();
  static int select(std::vector<SocketWithInfo *> *reads,
                    std::vector<SocketWithInfo *> *writes,
                    std::vector<SocketWithInfo *> *excepts, int timeout);
  std::string getIpAddress();
};

#endif