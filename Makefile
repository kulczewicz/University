Flags =-Wall

pracownia3 : pracownia3.cpp
	g++ $(Flags) -o pracownia3 pracownia3.cpp

run : pracownia3
	./pracownia3