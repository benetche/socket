# Internet Relay Chat 

Internet Relay Chat (IRC) implemented in C++ using RFC 1459 expecifications

## Group:
- Vitor Beneti Martins - 11877635

## Specifications:

- Operational System: GNU/Linux
- Kernel Release: 5.19.0-43-generic
- Architecture: x86_64
- Compiler: g++ (gcc) version 11.3.0 
- C++ Standard: C++11

## Instructions:
You'll probably need to install ncurses lib:
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
And readline lib:
```
sudo apt-get install libreadline-dev 
```
These libs are used for running a GUI.
  - You can compile both server and client by:
      ```
      make all
      ```
 - Or compile one by one:
    ```
    make server
    make client
    ```
		
  - First run server by:
      ```
      ./server
      ```
  - Then run client by:
      ```
      ./client
      ```
  - You can clear all generated files with:
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
    
