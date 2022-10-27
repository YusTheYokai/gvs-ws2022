OUT = build

default: all

all: clean build

clean:
	rm -rf $(OUT)

build: folderStructure command.o
	g++ -std=c++17 -Wall -I header -o $(OUT)/twmailer-client client/main.cpp $(OUT)/object/command.o
	g++ -std=c++17 -Wall -I header -o $(OUT)/twmailer-server server/main.cpp $(OUT)/object/command.o

folderStructure:
	mkdir -p $(OUT)/object

command.o:
	g++ -I header -o $(OUT)/object/command.o -c src/command.cpp
