#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <bits/stdc++.h>
#include <netdb.h>
/*
 * Biblioteca fornece funções e estruturas relacionadas à resolução de nomes de host,
 * obtenção de informações de endereço IP e outros recursos de rede. 
 *
 * Structs:
 *   - addrinfo: armazena informações de endereço retornadas por funções como getaddrinfo()
 */

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
  //parte da biblioteca netdb.h. Essa biblioteca fornece funções e estruturas relacionadas à resolução de nomes de host, obtenção de informações de endereço IP e outros recursos de rede. A estrutura addrinfo é usada para armazenar informações de endereço retornadas por funções como getaddrinfo().

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