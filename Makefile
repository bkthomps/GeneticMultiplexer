.DEFAULT_GOAL := clang

6mux:
	./genetic_multiplexer 6mux

11mux:
	./genetic_multiplexer 11mux

16middle3:
	./genetic_multiplexer 16middle3

clang:
	clang++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

gcc:
	g++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

clean:
	rm *.csv
	rm genetic_multiplexer
