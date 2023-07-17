#include "Socket.hpp"
#include "interface.hpp"
#include "util.hpp"


#include <arpa/inet.h>
/*
 * Biblioteca para manipulação de endereços IP.
 *
 * Constantes:
 *   - AF_INET: endereço IPv4.
 *   - AF_UNIX: endereço UNIX.
 *
 * Funções:
 *   - inet_ntop(): converte um endereço IP binário para uma string no formato decmia
 */

#include <errno.h>
/*
 * Biblioteca com a variável errno, utilizada para armazenar o código de erro
 * de uma função quando ocorre um erro.
 *
 * Constante:
 *   - errno: variável que armazena o código de erro.
 */

#include <fcntl.h>
/*
 * Biblioteca para controle de arquivos.
 *
 * Constantes:
 *   - O_NONBLOCK: indica que o descritor de arquivo deve ser colocado em modo não-bloqueante.
 */

#include <iostream>
/*
 * Biblioteca para controle de entrada e saída
 *
 * Função:
 *   - std::cout : saída para output.
 */

#include <netdb.h>
/*
 * Biblioteca para busca de nomes de domínio e números de serviço.
 *
 * Funções:
 *   - getaddrinfo(): converte nomes de host ou endereços de IP humanamente legíveis em uma
 *                    estrutura de endereço struct sockaddr utilizável para criar sockets.
 *   - gai_strerror(): retorna uma string de erro legível por humanos associada ao código de erro
 *                      retornado por getaddrinfo() ou getnameinfo().
 *   - freeaddrinfo(): libera a memória dinamicamente alocada pelo sistema para armazenar a
 *                      estrutura de endereços retornada por getaddrinfo().
 *   - getnameinfo(): converte uma estrutura de endereço struct sockaddr em um nome de host e
 *                     número de serviço humanamente legíveis, o oposto do que getaddrinfo() faz.
 */

#include <stdlib.h>
/*
 * Biblioteca padrão da linguagem C.
 *
 * Funções:
 *   - memset(): preenche um bloco de memória com um determinado valor.
 */

#include <string.h>
/*
 * Biblioteca para manipulação de strings.
 *
 * Função:
 *   - strerror(): retorna uma string que descreve o código de erro especificado em errno.
 */

#include <string>
/*
 * Biblioteca para utilização de strings em C++.
 *
 * Classe:
 *   - std::string: tipo de dado para representar uma string em C++.
 */

#include <sys/socket.h>
/*
 * Biblioteca para manipulação de sockets.
 *
 * Funções:
 *   - socket(): cria um socket.
 *   - bind(): associa um endereço a um socket.
 *   - connect(): inicia uma conexão em um socket.
 *   - listen(): coloca um socket em estado de escuta para aceitar conexões.
 *   - accept(): aceita uma conexão em um socket em estado de escuta.
 *   - send(): envia dados através de um socket.
 *   - recv(): recebe dados através de um socket.
 *   - setsockopt(): define opções para um socket.
 *   - getsockopt(): obtém opções de um socket.
 *   - getsockname(): obtém o endereço associado a um socket.
 *   - shutdown(): desativa uma parte específica de uma conexão de socket.
 */

#include <sys/un.h>
/*
 * Biblioteca para manipulação de sockets UNIX.
 *
 * Estruturas:
 *   - sockaddr_un: estrutura para representar um endereço de socket UNIX.
 */

#include <unistd.h>
/*
 * Biblioteca para utilização de chamadas de sistema relacionadas a POSIX.
 *
 * Função:
 *   - close(): fecha um descritor de arquivo.
 */

#include <vector>
/*
 * Biblioteca para utilização de vetores em C++.
 *
 * Classe:
 *   - std::vector: tipo de dado para representar um vetor em C++.
 */



// Parâmetros:
//   - domain: domínio do socket (por exemplo, AF_INET para IPv4).
//   - type: tipo do socket (por exemplo, SOCK_STREAM para TCP).
//   - protocol: protocolo utilizado pelo socket (por exemplo, IPPROTO_TCP para TCP).
//
// Comportamento:
//   - Utiliza a função memset() para preencher a estrutura de informações do endereço (addressInfo) com zeros.
//   - Chama a função socket() para criar um novo socket, passando os parâmetros domain, type e protocol.
//   - Verifica se o socket foi criado com sucesso. Caso contrário, chama a função safeExitFailure() para lidar com o erro.
//   - Atribui os valores domain, type e protocol à estrutura addressInfo.
//   - Inicializa as propriedades portNumber e ipAddress como strings vazias.

MySocket::MySocket(int domain, int type, int protocol) {
  memset(&addressInfo, 0, sizeof addressInfo);
  socketFD = socket(domain, type, protocol);
  if (socketFD == -1) {
    safeExitFailure("Error opening socket: " + std::string(strerror(errno)),
                    errno);
  }
  addressInfo.ai_family = domain;
  addressInfo.ai_socktype = type;
  addressInfo.ai_protocol = protocol;

  portNumber = "";
  ipAddress = "";
}

// MySocket::socketbind()
//
// Descrição: Associa o endereço IP e a porta especificados ao socket.
//
// Parâmetros:
//   - ip: endereço IP a ser associado ao socket.
//   - port: número da porta a ser associado ao socket.
//
// Retorno:
//   - status: inteiro representando o resultado da operação. Retorna 0 em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Atribui os valores de ip e port às propriedades ipAddress e portNumber, respectivamente.
//   - Verifica se o domínio do socket é AF_UNIX.
//     - Se for, cria uma estrutura sockaddr_un, preenche os campos necessários e chama a função bind() passando essa estrutura.
//     - Caso contrário, utiliza a função getaddrinfo() para obter informações do endereço com base em ip e port.
//       - Em caso de erro, chama a função safeExitFailure() para lidar com o erro.
//       - Atribui os valores retornados pela função getaddrinfo() à estrutura addressInfo.
//       - Libera a memória alocada pela função getaddrinfo() utilizando a função freeaddrinfo().
//       - Chama a função bind() passando a estrutura addressInfo.
//   - Verifica se ocorreu um erro na chamada à função bind(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status.

int MySocket::socketbind(std::string ip, std::string port) {
  this->ipAddress = ip;
  this->portNumber = port;
  int status;

  if (addressInfo.ai_family == AF_UNIX) {
    struct sockaddr_un address;
    memset(&address, 0, sizeof address);
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, ip.c_str(), sizeof(address.sun_path) - 1);
    status = ::bind(socketFD, (struct sockaddr *)&address, sizeof address);
  } else {
    struct addrinfo *ans;
    addressInfo.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(ip.c_str(), port.c_str(), &addressInfo, &ans)) !=
        0) {
      safeExitFailure("Error getting address info: " +
                          std::string(gai_strerror(status)),
                      status);
    }
    addressInfo.ai_addrlen = ans->ai_addrlen;
    addressInfo.ai_addr = ans->ai_addr;
    freeaddrinfo(ans);
    status = ::bind(socketFD, addressInfo.ai_addr, addressInfo.ai_addrlen);
  }
  if (status == -1) {
    safeExitFailure("Error binding socket: " + std::string(strerror(errno)),
                    errno);
  }
  return status;
}

// MySocket::socketConnect()
//
// Descrição: Estabelece uma conexão com o endereço IP e a porta especificados.
//
// Parâmetros:
//   - ip: endereço IP ao qual se deseja conectar.
//   - port: número da porta à qual se deseja conectar.
//
// Retorno:
//   - status: 0 em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Atribui os valores de ip e port às propriedades ipAddress e portNumber, respectivamente.
//   - Inicializa status como 0.
//   - Verifica se o domínio do socket é AF_UNIX.
//     - Se for, cria uma estrutura sockaddr_un, preenche os campos necessários e chama a função connect() passando essa estrutura.
//     - Caso contrário, utiliza a função getaddrinfo() para obter informações do endereço com base em ip e port.
//       - Em caso de erro, chama a função safeExitFailure() para lidar com o erro.
//       - Atribui os valores retornados pela função getaddrinfo() à estrutura addressInfo.
//       - Libera a memória alocada pela função getaddrinfo() utilizando a função freeaddrinfo().
//       - Chama a função connect() passando a estrutura addressInfo.
//       - Verifica se ocorreu um erro na chamada à função connect().
//         - Se o erro for EINPROGRESS, significa que a conexão está em andamento.
//           - Cria um objeto SocketWithInfo temporário para uso no seletor.
//           - Chama a função MySocket::select() para aguardar até que a conexão seja estabelecida ou ocorra um erro.
//           - Verifica o resultado do seletor e trata os casos possíveis.
//           - Deleta o objeto SocketWithInfo temporário.
//         - Se o erro for ECONNREFUSED, significa que a conexão foi recusada.
//     - Se ocorreu um erro na chamada à função connect() ou no seletor, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status.

int MySocket::socketConnect(std::string ip, std::string port) {
  this->ipAddress = ip;
  this->portNumber = port;
  int status = 0;

  if (addressInfo.ai_family == AF_UNIX) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, ip.c_str(), sizeof(addr.sun_path) - 1);
    status = ::connect(socketFD, (struct sockaddr *)&addr, sizeof(addr));

  } else {

    struct addrinfo *ans;

    if ((status = getaddrinfo(ip.c_str(), port.c_str(), &addressInfo, &ans)) !=
        0) {
      if (status == EAI_NONAME) {
        return status;
      }
      safeExitFailure("Error getting address info: " +
                          std::string(gai_strerror(status)),
                      status);
    }
    addressInfo.ai_addrlen = ans->ai_addrlen;
    addressInfo.ai_addr = ans->ai_addr;
    freeaddrinfo(ans);
    status = ::connect(socketFD, addressInfo.ai_addr, addressInfo.ai_addrlen);

    if (status == -1) {
      if (errno == EINPROGRESS) {
        SocketWithInfo *tmpWithInfo = new SocketWithInfo(this, true);

        std::vector<SocketWithInfo *> writes(1);
        writes[0] = tmpWithInfo;

        int nWrites = MySocket::select(nullptr, &writes, nullptr, 5);

        std::cout << "nWrites: " << nWrites << std::endl;

        if (nWrites < 0 && errno != EINTR) {
          status = -1;
        } else if (nWrites > 0) {
          int error;
          socklen_t len = sizeof(error);

          if (getsockopt(socketFD, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
            safeExitFailure("Error getting socket option: " +
                                std::string(strerror(errno)),
                            errno);
          }

          if (error != 0) {
            if (error == ECONNREFUSED) {
              status = ECONNREFUSED;
            } else {
              status = -1;
              errno = error;
            }
          } else {
            status = 0;
          }

        } else {
          status = ETIMEDOUT;
        }

        delete tmpWithInfo;
      } else if (errno == ECONNREFUSED) {
        status = ECONNREFUSED;
      }
    }
  }

  if (status == -1) {
    safeExitFailure("Error connecting socket: " + std::string(strerror(errno)),
                    errno);
  }
  return status;
}


// Descrição: Coloca o socket em modo de escuta para aguardar conexões.
//
// Parâmetros:
//   - maxQueue: tamanho máximo da fila de conexões pendentes.
//
// Retorno:
//   - status: 0 em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Chama a função listen() para colocar o socket em modo de escuta, especificando o tamanho máximo da fila de conexões pendentes (maxQueue).
//   - Verifica se ocorreu um erro na chamada à função listen(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status.

int MySocket::socketListen(int maxQueue) {
  int status;
  status = ::listen(socketFD, maxQueue);
  if (status == -1) {
    safeExitFailure(
        "Error listening on socket: " + std::string(strerror(errno)), errno);
  }
  return status;
}

// MySocket::accept()
//
// Descrição: Aceita uma conexão em um socket e cria um novo objeto MySocket para representar a nova conexão.
//
// Retorno:
//   - newSocket: ponteiro para um objeto MySocket que representa a nova conexão aceita.
//
// Comportamento:
//   - Cria uma estrutura sockaddr_storage e uma variável otherAddrLen para armazenar informações sobre o endereço do cliente.
//   - Chama a função accept() para aceitar uma conexão no socket e obter um novo descritor de socket para a conexão aceita.
//   - Verifica se ocorreu um erro na chamada à função accept(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Cria um novo objeto MySocket com base nas informações de domínio, tipo e protocolo do socket original.
//   - Atribui o novo descritor de socket (newSocketFD) e o número da porta ao novo objeto MySocket.
//   - Obtém o endereço IP do cliente usando a função getnameinfo().
//     - Em caso de erro, chama a função safeExitFailure() para lidar com o erro.
//   - Atribui o endereço IP ao novo objeto MySocket e atualiza a família do endereço e o ponteiro para o endereço na estrutura addressInfo do novo objeto.
//   - Retorna o ponteiro para o novo objeto MySocket.

MySocket *MySocket::accept() {
  struct sockaddr_storage otherAddr;
  socklen_t otherAddrLen = sizeof otherAddr;
  int newSocketFD =
      ::accept(socketFD, (struct sockaddr *)&otherAddr, &otherAddrLen);
  if (newSocketFD == -1) {
    safeExitFailure("Error accepting socket: " + std::string(strerror(errno)),
                    errno);
  }
  MySocket *newSocket = new MySocket(
      addressInfo.ai_family, addressInfo.ai_socktype, addressInfo.ai_protocol);
  newSocket->socketFD = newSocketFD;
  newSocket->portNumber = portNumber;

  char host[NI_MAXHOST];
  int status = getnameinfo((struct sockaddr *)&otherAddr, otherAddrLen, host,
                           sizeof host, NULL, 0, NI_NUMERICHOST);
  if (status == -1) {
    safeExitFailure("Error getting hostname: " + std::string(strerror(errno)),
                    errno);
  }
  newSocket->ipAddress = host;
  newSocket->addressInfo.ai_family = otherAddr.ss_family;
  newSocket->addressInfo.ai_addr = (struct sockaddr *)&otherAddr;
  return newSocket;
}



// Parâmetros:
//   - message: mensagem a ser enviada pelo socket.
//
// Retorno:
//   - status: número de bytes enviados em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Obtém o código de erro (error) e seu tamanho (len) utilizando a função getsockopt(), passando o socketFD, SOL_SOCKET e SO_ERROR.
//   - Verifica se ocorreu um erro na chamada à função getsockopt() ou se o código de erro não é zero.
//     - Em caso afirmativo, retorna -2 para indicar um erro.
//   - Chama a função send() para enviar a mensagem pelo socket, passando o socketFD, a mensagem convertida para uma sequência de caracteres, o tamanho da mensagem e a flag 0.
//   - Verifica se ocorreu um erro na chamada à função send(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status, que representa o número de bytes enviados.
int MySocket::socketWrite(std::string message) {

  int error = 0;
  socklen_t len = sizeof(error);
  int ret = getsockopt(socketFD, SOL_SOCKET, SO_ERROR, &error, &len);

  if (ret != 0 || error != 0) {
    return -2;
  }

  int status = (int)send(socketFD, message.c_str(), message.size(), 0);

  if (status == -1) {
    safeExitFailure("Error writing to socket: " + std::string(strerror(errno)),
                    errno);
  }
  return status;
}


// Parâmetros:
//   - buffer: referência para uma string onde os dados lidos serão armazenados.
//   - length: tamanho máximo a ser lido do socket.
//
// Retorno:
//   - status: Retorna o número de bytes lidos em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Aloca um buffer temporário de caracteres (char) com o tamanho especificado.
//   - Limpa o buffer temporário.
//   - Chama a função recv() para ler dados do socket, passando o socketFD, o buffer temporário, o tamanho a ser lido (length - 1) e a flag 0.
//   - Verifica se ocorreu um erro na chamada à função recv(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Copia o conteúdo do buffer temporário para a string de destino (buffer).
//   - Libera a memória alocada para o buffer temporário.
//   - Retorna o valor de status, que representa o número de bytes lidos.

int MySocket::socketRead(std::string &buffer, int length) {
  char *buff = new char[length];
  memset(buff, 0, length);
  int status = (int)recv(socketFD, buff, length - 1, 0);
  if (status == -1) {
    safeExitFailure(
        "Error reading from socket: " + std::string(strerror(errno)), errno);
  }

  buffer = std::string(buff);
  delete[] buff;
  return status;
}

// Parâmetros:
//   - buffer: referência para uma string onde os dados lidos serão armazenados.
//   - length: tamanho máximo a ser lido do socket.
//   - timeout: tempo máximo em segundos para esperar por dados no socket.
//
// Retorno:
//   - status: inteiro representando o resultado da operação. Retorna o número de bytes lidos em caso de sucesso, -1 em caso de timeout e -2 em caso de erro.
//
// Comportamento:
//   - Cria um vetor de ponteiros SocketWithInfo para leitura (reads) e adiciona um elemento ao vetor contendo informações do socket atual.
//   - Chama a função select() para esperar até que o socket esteja pronto para leitura, usando o vetor reads, nullptr para o vetor de escrita (writes) e exceção (excepts), e o tempo limite especificado.
//   - Verifica se ocorreu um timeout, retornando -1 se nenhum socket estiver pronto para leitura.
//   - Aloca um buffer temporário de caracteres (char) com o tamanho especificado.
//   - Limpa o buffer temporário.
//   - Chama a função recv() para ler dados do socket, passando o socketFD, o buffer temporário, o tamanho a ser lido (length - 1) e a flag 0.
//   - Verifica se ocorreu um erro na chamada à função recv(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Copia o conteúdo do buffer temporário para a string de destino (buffer).
//   - Libera a memória alocada para o buffer temporário.
//   - Retorna o valor de status, que representa o número de bytes lidos.
int MySocket::socketSafeRead(std::string &buffer, int length, int timeout) {
  std::vector<SocketWithInfo *> reads;
  auto clientInfo = new SocketWithInfo(this, false);
  reads.push_back(clientInfo);
  int count = MySocket::select(&reads, nullptr, nullptr, timeout);

  delete clientInfo;

  if (count < 1) {

    buffer = "";
    return -1;
  }

  char *buff = new char[length];
  memset(buff, 0, length);
  int status = (int)recv(socketFD, buff, length - 1, 0);
  if (status < 0) {
    safeExitFailure(
        "Error reading from socket: " + std::string(strerror(errno)), errno);
  }
  buffer = std::string(buff);
  delete[] buff;
  return status;
}

// Parâmetros:
//   - level: nível no qual a opção é definida.
//   - optName: nome da opção a ser definida.
//   - optVal: uponteiro para o valor da opção.
//
// Retorno:
//   - status: inteiro representando o resultado da operação. Retorna 0 em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Chama a função setsockopt() para definir uma opção do socket, passando o socketFD, o nível (level), o nome da opção (optName), o valor da opção (optVal) e o tamanho do valor.
//   - Verifica se ocorreu um erro na chamada à função setsockopt(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status, que representa o resultado da operação.
int MySocket::socketSetOpt(int level, int optName, void *optVal) {

  int status = ::setsockopt(socketFD, level, optName, optVal, sizeof optVal);
  if (status == -1) {
    safeExitFailure(
        "Error setting socket option: " + std::string(strerror(errno)), errno);
  }
  return status;
}

// Parâmetros:
//   - level: nível no qual a opção está definida.
//   - optName: nome da opção a ser obtida.
//   - optVal: ponteiro para o valor da opção.
//
// Retorno:
//   - status: inteiro representando o resultado da operação. Retorna 0 em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Obtém o tamanho da opção (lentgh) utilizando a função sizeof() com o ponteiro optVal.
//   - Chama a função getsockopt() para obter o valor de uma opção do socket, passando o socketFD, o nível (level), o nome da opção (optName), o ponteiro para o valor da opção (optVal) e o tamanho da opção.
//   - Verifica se ocorreu um erro na chamada à função getsockopt(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status, que representa o resultado da operação.
int MySocket::socketGetOpt(int level, int optName, void *optVal) {
  socklen_t lentgh = sizeof optVal;
  int status = ::getsockopt(socketFD, level, optName, optVal, &lentgh);
  if (status == -1) {
    safeExitFailure(
        "Error getting socket option: " + std::string(strerror(errno)), errno);
  }
  return status;
}


// Comportamento:
//   - Chama a função close() para fechar o socketFD.
void MySocket::close() { ::close(socketFD); }


// Parâmetros:
//   - blocking: booleano que indica se o socket deve ser definido como bloqueante (true) ou não-bloqueante (false).
//
// Retorno:
//   - status: inteiro representando o resultado da operação. Retorna o novo status do socket em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Obtém o status atual do socket chamando a função fcntl() com o socketFD e F_GETFL.
//   - Verifica se ocorreu um erro na chamada à função fcntl(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Se o valor de blocking for true, remove a flag O_NONBLOCK do status. Caso contrário, adiciona a flag O_NONBLOCK ao status.
//   - Chama a função fcntl() novamente para definir o novo status do socket.
//   - Verifica se ocorreu um erro na segunda chamada à função fcntl(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o novo valor de status, que representa o status atualizado do socket.

int MySocket::setBlocking(bool blocking) {
  long status = fcntl(socketFD, F_GETFL, NULL);

  if (status < 0) {
    safeExitFailure("Error getting socket flags to set to nonblocking: " +
                        std::string(strerror(errno)),
                    errno);
  }

  if (blocking) {
    status &= ~O_NONBLOCK;
  } else {
    status |= O_NONBLOCK;
  }

  status = fcntl(socketFD, F_SETFL, status);

  if (status < 0) {
    safeExitFailure("Error setting socket flags to nonblocking: " +
                        std::string(strerror(errno)),
                    errno);
  }
  return (int)status;
}

// MySocket::socketShutdown(int how)
//
// Descrição: Desliga uma parte da conexão do socket.
//
// Parâmetros:
//   - how: parte da conexão a ser desligada (SHUT_RD, SHUT_WR ou SHUT_RDWR).
//
// Retorno:
//   - status: inteiro representando o resultado da operação. Retorna 0 em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Chama a função shutdown() para desligar uma parte da conexão do socket, passando o socketFD e a parte da conexão (how).
//   - Verifica se ocorreu um erro na chamada à função shutdown(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Retorna o valor de status, que representa o resultado da operação.


int MySocket::socketShutdown(int how) {
  int status = ::shutdown(socketFD, how);

  if (status < 0) {
    safeExitFailure(
        "Error shutting down socket: " + std::string(strerror(errno)), errno);
  }
  return status;
}

// Descrição: Espera até que um ou mais sockets estejam prontos para leitura, escrita ou exceção.
//
// Parâmetros:
//   - reads: ponteiro para um vetor de ponteiros SocketWithInfo com informações sobre os sockets para leitura.
//   - writes: ponteiro para um vetor de ponteiros SocketWithInfo com informações sobre os sockets para escrita.
//   - excepts: ponteiro para um vetor de ponteiros SocketWithInfo com informações sobre os sockets para exceção.
//   - timeout: tempo máximo em segundos a esperar pelos sockets.
//
// Retorno:
//   - result: número de sockets prontos em caso de sucesso e -1 em caso de erro.
//
// Comportamento:
//   - Declara e inicializa variáveis e estruturas necessárias para a chamada à função select(), como timeval, fd_set e maxSock.
//   - Define o valor de timeout na estrutura timeval.
//   - Limpa as estruturas fd_set para leitura, escrita e exceção.
//   - Determina o valor máximo do socket entre os sockets fornecidos em reads, writes e excepts.
//   - Adiciona cada socket de leitura, escrita e exceção às respectivas estruturas fd_set.
//   - Chama a função select() com o valor máximo do socket + 1, as estruturas fd_set, e a estrutura timeval.
//   - Verifica se ocorreu um erro na chamada à função select(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Remove os sockets que não estão prontos para leitura, escrita ou exceção dos respectivos vetores.
//   - Retorna o valor de result, que representa o número de sockets prontos.
int MySocket::select(std::vector<SocketWithInfo *> *reads,
                     std::vector<SocketWithInfo *> *writes,
                     std::vector<SocketWithInfo *> *excepts, int timeout) {

  struct timeval timeValue;
  fd_set readFDs;
  fd_set writeFDs;
  fd_set exceptFDs;

  timeValue.tv_sec = timeout;
  timeValue.tv_usec = 0;

  FD_ZERO(&readFDs);

  FD_ZERO(&writeFDs);

  FD_ZERO(&exceptFDs);

  int maxSock = 0;

  if (reads != nullptr) {
    for (size_t i = 0; i < reads->size(); i++) {
      // ERROR IS HERE
      int sockInt = reads->at(i)->socket->socketFD;

      if (sockInt > maxSock) {
        maxSock = sockInt;
      }
      FD_SET(sockInt, &readFDs);
    }
  }

  if (writes != nullptr) {
    for (size_t i = 0; i < writes->size(); i++) {
      int sockInt = writes->at(i)->socket->socketFD;
      if (sockInt > maxSock) {
        maxSock = sockInt;
      }
      FD_SET(sockInt, &writeFDs);
    }
  }

  if (excepts != nullptr) {
    for (size_t i = 0; i < excepts->size(); i++) {
      int sockInt = excepts->at(i)->socket->socketFD;
      if (sockInt > maxSock) {
        maxSock = sockInt;
      }
      FD_SET(sockInt, &exceptFDs);
    }
  }

  int result =
      ::select(maxSock + 1, &readFDs, &writeFDs, &exceptFDs, &timeValue);

  if (result < 0) {
    safeExitFailure("Error in select: " + std::string(strerror(errno)), errno);
  }
  if (reads != nullptr) {
    for (int i = (int)reads->size() - 1; i >= 0; i--) {
      if (!FD_ISSET(reads->at(i)->socket->socketFD, &readFDs)) {
        reads->erase(reads->begin() + i);
      }
    }
  }
  if (writes != nullptr) {
    for (int i = (int)writes->size() - 1; i >= 0; i--) {
      if (!FD_ISSET(writes->at(i)->socket->socketFD, &writeFDs)) {
        writes->erase(writes->begin() + i);
      }
    }
  }
  if (excepts != nullptr) {
    for (int i = (int)excepts->size() - 1; i >= 0; i--) {
      if (!FD_ISSET(excepts->at(i)->socket->socketFD, &exceptFDs)) {
        excepts->erase(excepts->begin() + i);
      }
    }
  }
  return result;
}


// Retorno:
//   - ip: string contendo o endereço IP do socket.
//
// Comportamento:
//   - Declara uma estrutura sockaddr_in e um tamanho (len) para armazenar informações do socket.
//   - Chama a função getsockname() para obter o endereço do socket, passando o socketFD, um ponteiro para a estrutura sockaddr_in e um ponteiro para o tamanho.
//   - Verifica se ocorreu um erro na chamada à função getsockname(). Em caso afirmativo, chama a função safeExitFailure() para lidar com o erro.
//   - Converte o endereço IP em formato binário para uma string usando a função inet_ntop().
//   - Retorna a string que representa o endereço IP.


std::string MySocket::getIpAddress() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if (getsockname(socketFD, (struct sockaddr *)&addr, &len) < 0) {
    safeExitFailure(
        "Error getting socket address: " + std::string(strerror(errno)), errno);
  }
  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
  return std::string(ip);
}

// Parâmetros:
//   - socket: um ponteiro para o objeto MySocket associado ao SocketWithInfo.
//   - isClient: um valor booleano que indica se o SocketWithInfo representa um cliente ou não.
//
// Comportamento:
//   - Atribui o valor do ponteiro MySocket recebido ao atributo socket.
//   - Atribui o valor de isClient ao atributo isClient.
SocketWithInfo::SocketWithInfo(MySocket *socket, bool isClient) {
  this->socket = socket;
  this->isClient = isClient;
}