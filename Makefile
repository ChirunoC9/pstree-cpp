CC = clang++
CCFLAG = -Wall -Wextra -pipe
CCDEBUG = -ggdb
CCRELEASE = -O2
CCSTD = -std=c++23
CCASAN = -fsanitize=address,undefined

pstree.out: pstree.cpp
	$(CC) pstree.cpp -o pstree.out $(CCSTD) $(CCFLAG) $(CCRELEASE)

run: pstree.out
	./pstree.out

clean:
	rm -f ./pstree.out

.PHONY: clean
