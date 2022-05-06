CC=g++
CFLAG=-I
DEPS = PatternMatching.h output.h

PMiner: main.cpp PatternMatching.cpp
	g++ main.cpp PatternMatching.cpp -ltbb -o main