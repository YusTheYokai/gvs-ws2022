OUT = build
GLOBAL_INCLUDE = header
GLOBAL_OBJECT = $(OUT)/object/command.o $(OUT)/object/messageUtils.o

default: all

all: clean build

clean:
	rm -rf $(OUT)

build: folderStructure command.o messageUtils.o
	g++ -std=c++17 -Wall -I $(GLOBAL_INCLUDE) -o $(OUT)/twmailer-client client/main.cpp $(GLOBAL_OBJECT)
	g++ -std=c++17 -Wall -I $(GLOBAL_INCLUDE) -o $(OUT)/twmailer-server server/main.cpp $(GLOBAL_OBJECT)

folderStructure:
	mkdir -p $(OUT)/object

command.o:
	g++ -I header -o $(OUT)/object/command.o -c src/command.cpp

messageUtils.o:
	g++ -I header -o $(OUT)/object/messageUtils.o -c src/messageUtils.cpp
