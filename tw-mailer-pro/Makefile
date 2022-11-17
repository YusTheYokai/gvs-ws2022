CC             = g++
CFLAGS         = -std=c++17 -Wall

OUT            = build
GLOBAL_INCLUDE = header
GLOBAL_OBJECT  = $(OUT)/object/command.o $(OUT)/object/messageUtils.o $(OUT)/object/getoptUtils.o $(OUT)/object/logger.o

CLIENT_INCLUDE = client/header
CLIENT_OBJECT  = $(OUT)/object/usernameUtils.o



default: all

all: clean build

clean:
	rm -rf $(OUT)

build: folderStructure command.o getoptUtils.o logger.o messageUtils.o usernameUtils.o
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE) -I $(CLIENT_INCLUDE) -o $(OUT)/twmailer-client client/main.cpp $(GLOBAL_OBJECT) $(CLIENT_OBJECT)
	$(CC) $(CFLAGS) -I $(GLOBAL_INCLUDE)                      -o $(OUT)/twmailer-server server/main.cpp $(GLOBAL_OBJECT)

folderStructure:
	mkdir -p $(OUT)/object

command.o:
	$(CC) $(CFLAGS) -I header        -o $(OUT)/object/command.o       -c src/command.cpp

getoptUtils.o:
	$(CC) $(CFLAGS) -I header        -o $(OUT)/object/getoptUtils.o   -c src/getoptUtils.cpp

logger.o:
	$(CC) $(CFLAGS) -I header        -o $(OUT)/object/logger.o        -c src/logger.cpp

messageUtils.o:
	$(CC) $(CFLAGS) -I header        -o $(OUT)/object/messageUtils.o  -c src/messageUtils.cpp

usernameUtils.o:
	$(CC) $(CFLAGS) -I client/header -o $(OUT)/object/usernameUtils.o -c client/src/usernameUtils.cpp
