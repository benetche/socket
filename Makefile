CC=g++
LD=g++

CFLAGS= -std=c++11 -pthread -Wall -Wextra -Werror -pedantic -g -O0
LIBS=-lm -lstdc++ -lncurses -lreadline -lpthread
DLDFLAGS=-g

SRCS    := $(wildcard ./*.cpp)
SERVER_SRCS := $(filter-out %/clientMain.cpp,$(SRCS))
CLIENT_SRCS := $(filter-out %/serverMain.cpp,$(SRCS))
SERVER_OBJS    := $(patsubst ./%.cpp,./%.o,$(SERVER_SRCS))
CLIENT_OBJS    := $(patsubst ./%.cpp,./%.o,$(CLIENT_SRCS))

./%.o: ./%.cpp ./%.hpp
	$(CC) $(CFLAGS) -c $< -o $@

all: server client

server: $(SERVER_OBJS)
	$(LD) $^ -o server $(LIBS)

client: $(CLIENT_OBJS)
	$(LD) $^ -o client $(LIBS)
clean:
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_FILE) $(CLIENT_FILE) vgcore*

zip:
	zip -r main.zip LICENSE README.md Makefile *.hpp *.cpp