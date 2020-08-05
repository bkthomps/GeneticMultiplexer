.DEFAULT_GOAL := compile

11mux:
	./genetic_multiplexer 11mux

16mux:
	./genetic_multiplexer 16mux

16middle3:
	./genetic_multiplexer 16middle3

compile:
	clang++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

clean:
	rm *.csv
	rm genetic_multiplexer
