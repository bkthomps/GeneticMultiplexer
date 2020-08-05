.DEFAULT_GOAL := compile

run:
	./genetic_multiplexer

compile:
	clang++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

clean:
	rm *.csv
	rm genetic_multiplexer
