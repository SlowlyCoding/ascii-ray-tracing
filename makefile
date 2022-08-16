default:
	g++ src/main.cpp -o output -std=c++11 -pthread
	./output

debug:
	g++ src/main.cpp -o output -std=c++11 -pthread -g
	gdb ./output

