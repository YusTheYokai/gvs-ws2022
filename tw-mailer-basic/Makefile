default: all

all: clean build

clean:
	rm -f twmailer-client twmailer-server

build:
	g++ client/main.cpp -o twmailer-client -std=c++17 -Wall
	g++ server/main.cpp -o twmailer-server -std=c++17 -Wall
