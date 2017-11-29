Flags = -pthread -Wall -Werror

osadnicy : osadnicy.cpp
	g++ $(Flags) -o osadnicy osadnicy.cpp

run : osadnicy
	./osadnicy 10 2 10 10
