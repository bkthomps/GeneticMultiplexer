.DEFAULT_GOAL := clang

clang:
	clang++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

gcc:
	g++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o genetic_multiplexer

test:
	make clang
	clang++ tst/integration.cpp --std=c++17 -o test_gen_mux
	./genetic_multiplexer 4
	./test_gen_mux 4_data_pins_tree.csv

long_test:
	make clang
	clang++ tst/integration.cpp --std=c++17 -o test_gen_mux
	./genetic_multiplexer 8
	./test_gen_mux 8_data_pins_tree.csv

clean:
	rm -f *.csv
	find *.txt -type f ! -name 'CMakeLists.txt' -delete
	rm -f genetic_multiplexer
	rm -f test_gen_mux
