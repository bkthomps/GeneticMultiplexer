.DEFAULT_GOAL := clang

clang:
	clang++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o gen_mux

gcc:
	g++ src/expressions.cpp src/main.cpp --std=c++17 -O3 -o gen_mux

test: clang
	clang++ tst/integration.cpp --std=c++17 -o test_gen_mux
	./gen_mux 2
	./test_gen_mux 2_address_pins_tree.txt

long_test: clang
	clang++ tst/integration.cpp --std=c++17 -o test_gen_mux
	./gen_mux 3
	./test_gen_mux 3_address_pins_tree.txt

clean:
	rm -f *.csv
	find *.txt -type f ! -name 'CMakeLists.txt' -delete
	rm -f gen_mux
	rm -f test_gen_mux
