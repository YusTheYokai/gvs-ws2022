CC             = g++
CFLAGS         = -std=c++2a -Wall

OUT            = build
GLOBAL_INCLUDE = header
GLOBAL_OBJECT  = $(OUT)/object/command.o $(OUT)/object/messageUtils.o $(OUT)/object/getoptUtils.o $(OUT)/object/logger.o

CLIENT_INCLUDE = client/header
CLIENT_OBJECT  = $(OUT)/object/clientCommand.o $(OUT)/object/userUtils.o

SERVER_INCLUDE = server/header
SERVER_OBJECT  = $(OUT)/object/ldapClass.o $(OUT)/object/ldapUtils.o

default: all

all: clean build

clean:
	rm -rf $(OUT)

build: folderStructure clientCommand.o command.o getoptUtils.o ldapClass.o ldapUtils.o logger.o messageUtils.o userUtils.o
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE) -I $(CLIENT_INCLUDE) -o $(OUT)/twmailer-client client/main.cpp $(GLOBAL_OBJECT) $(CLIENT_OBJECT)
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE) -I $(SERVER_INCLUDE) -o $(OUT)/twmailer-server server/main.cpp $(GLOBAL_OBJECT) $(SERVER_OBJECT) -pthread -lldap -llber

folderStructure:
	mkdir -p $(OUT)/object

clientCommand.o:
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE) -I $(CLIENT_INCLUDE) -o $(OUT)/object/clientCommand.o -c client/src/clientCommand.cpp 

command.o:
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE)                      -o $(OUT)/object/command.o       -c src/command.cpp

getoptUtils.o:
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE)                      -o $(OUT)/object/getoptUtils.o   -c src/getoptUtils.cpp

ldapClass.o:
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE) -I $(SERVER_INCLUDE) -o $(OUT)/object/ldapClass.o     -c server/src/ldapClass.cpp

ldapUtils.o:
	$(CC) $(CFLAGS) -I $(SERVER_INCLUDE)                      -o $(OUT)/object/ldapUtils.o     -c server/src/ldapUtils.cpp

logger.o:
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE)                      -o $(OUT)/object/logger.o        -c src/logger.cpp

messageUtils.o:
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE)                      -o $(OUT)/object/messageUtils.o  -c src/messageUtils.cpp

userUtils.o:
	$(CC) $(CFLAGS) -I $(CLIENT_INCLUDE)                      -o $(OUT)/object/userUtils.o     -c client/src/userUtils.cpp
