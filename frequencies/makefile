.PHONY: clean main test

main: freq

dictionary.o: dictionary.h dictionary.cpp
	g++ -c -Wall dictionary.cpp -O2 -std=c++17

counter.o: counter.h counter.cpp
	g++ -c -Wall counter.cpp -O2 -std=c++17

freq: main.cpp dictionary.o counter.o
	g++ -Wall main.cpp dictionary.o counter.o -O2 -std=c++17 -o freq

test: tests.cpp dictionary.o counter.o
	g++ -Wall tests.cpp dictionary.o counter.o -O2 -std=c++17 -o test
	./test

clean:
	rm -f *.o
	rm -f freq
	rm -f test
