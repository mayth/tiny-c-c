PROG := ylaciexpr list-test trie-test

AST_SRCS := AST.h AST.c
TRIE_SRCS := trie.h trie.c
LIST_SRCS := list.h list.c
EXPR_SRCS := expr.y lexer.c interpreter.c
SRCS := $(AST_SRCS) $(TRIE_SRCS) $(LIST_SRCS) $(EXPR_SRCS)

CC := clang
CFLAGS=-Wall -std=c99

ylaciexpr: expr.o AST.o trie.o list.o interpreter.c
	$(CC) $(CFLAGS) -o ylaciexpr expr.o AST.o trie.o list.o interpreter.c

test-components: list-test trie-test

list-test: list.o list_test.c
	$(CC) $(CFLAGS) -o list-test list.o list_test.c

trie-test: trie.o trie_test.c
	$(CC) $(CFLAGS) -o trie-test trie.o trie_test.c

all: $(PROG)

AST.o: $(AST_SRCS) trie.o list.o
	$(CC) $(CFLAGS) -c AST.c

trie.o: $(TRIE_SRCS)
	$(CC) $(CFLAGS) -c trie.c

list.o: $(LIST_SRCS)
	$(CC) $(CFLAGS) -c list.c

expr.o: $(EXPR_SRCS)
	yacc $(YACCFLAGS) expr.y
	mv y.tab.c expr.c
	$(CC) $(CFLAGS) -c expr.c

tarball:
	mkdir ylaciexpr
	tar zcvf ylaciexpr.tar.gz $(SRCS) list_test.c trie_test.c Makefile LICENSE.md README.md
	rm -r ylaciexpr

clean:
	rm -f *.o expr.c *~ $(PROG)
