.DEFAULT_GOAL := clang

clang:
	clang++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

gcc:
	g++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

clean:
	rm *.csv
	rm genetic_multiplexer
