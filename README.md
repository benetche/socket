# ircCppImplementation

IRC Implementation in CPP

Implementation made for the discipline of Computer Networks - SSC0142 at ICMC - USP

## Group:
- Vitor Beneti Martins - 11877635

## Specifications:

- Operational System: GNU/Linux
- Kernel Release: 5.19.0-43-generic
- Architecture: x86_64
- Compiler: g++ (gcc) version 11.3.0 
- C++ Standard: C++11

## Instructions:

 - To compile the server run the following command:
    ```
    make server
    ```
  - To compile the client run the following command:
      ```
      make client
      ```
  - To run the server run the following command:
      ```
      ./server
      ```
  - To run the client run the following command:
      ```
      ./client
      ```
  - To clear the compiled files run the following command:
      ```
      make clean
      ```

## Commands:
|**Command**|**Description**|**Permission**|
|-----------|-------------|-------------|
|`/connect <address>`|Connects Client to Server|All Users|
|`/quit`|Finishes connection between client and server|All Users|
|`/ping`|Checks connection between client and server|All Users|
|`/join <channel>`|Enters a channel or create it if doesn't exists `<channel>`|All Users|
|`/nickname <nickname>`|Changes current nickname|All Users|
|`/kick <nickname>`|Kicks an user from current channel|Admin|
|`/mute <nickname>`|Prevents an user from sending messages in current channel|Admin|
|`/unmute <nickname>`|Unmute a muted user in the channel|Admin|
|`/whois <nickname>`|Check IP address from a given user|Admin|

## Presentation Video:
    