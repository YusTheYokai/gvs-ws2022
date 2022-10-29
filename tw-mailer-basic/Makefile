OUT = build
GLOBAL_INCLUDE = header
GLOBAL_OBJECT = $(OUT)/object/command.o $(OUT)/object/messageUtils.o

CLIENT_INCLUDE = client/header
CLIENT_OBJECT = $(OUT)/object/usernameUtils.o



default: all

all: clean build

clean:
	rm -rf $(OUT)

build: folderStructure command.o messageUtils.o usernameUtils.o
	g++ -std=c++17 -Wall -I $(GLOBAL_INCLUDE) -I $(CLIENT_INCLUDE) -o $(OUT)/twmailer-client client/main.cpp $(GLOBAL_OBJECT) $(CLIENT_OBJECT)
	g++ -std=c++17 -Wall -I $(GLOBAL_INCLUDE) -o $(OUT)/twmailer-server server/main.cpp $(GLOBAL_OBJECT)

folderStructure:
	mkdir -p $(OUT)/object

command.o:
	g++ -std=c++17 -Wall -I header -o $(OUT)/object/command.o -c src/command.cpp

messageUtils.o:
	g++ -std=c++17 -Wall -I header -o $(OUT)/object/messageUtils.o -c src/messageUtils.cpp

usernameUtils.o:
	g++ -std=c++17 -Wall -I client/header -o $(OUT)/object/usernameUtils.o -c client/src/usernameUtils.cpp
