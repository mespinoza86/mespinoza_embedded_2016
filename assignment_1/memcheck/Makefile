build_lib:
	gcc -shared -fPIC -o ./lib/libmemcheck.so ./lib/libmemcheck.c -ldl	

build_mem:
	gcc -o ./src/memcheck ./src/memcheck.c

run_mem:
	./src/memcheck -h

clean_all:
	rm -rf ./src/memcheck
	rm -rf ./lib/libmemcheck.so


