CC=g++
CFLAG=-I
DEPS = PMiner.h output.h

PMiner: main.cpp PMiner.cpp
	g++ main.cpp PMiner.cpp -ltbb -o ./bin/count