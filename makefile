CC=g++
CFLAG=-I
DEPS = PMiner.h output.h

PMiner: ./src/main.cpp ./src/PMiner.cpp
	g++ ./src/main.cpp ./src/PMiner.cpp -ltbb -o ./bin/PMiner