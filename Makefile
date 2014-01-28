PROG := tiny-c list-test trie-test stack-test

AST_SRCS := AST.h AST.c
TRIE_SRCS := trie.h trie.c
LIST_SRCS := list.h list.c
STACK_SRCS := stack.h stack.c
EXPR_SRCS := parser.y lexer.c interpreter.c
SRCS := $(AST_SRCS) $(TRIE_SRCS) $(LIST_SRCS) $(STACK_SRCS) $(EXPR_SRCS)

CC := clang
CFLAGS=-Wall -std=c99 -g

tiny-c: parser.o AST.o trie.o list.o interpreter.c
	$(CC) $(CFLAGS) -o tiny-c parser.o AST.o trie.o list.o interpreter.c

test-components: list-test trie-test stack-test

list-test: list.o list_test.c
	$(CC) $(CFLAGS) -o list-test list.o list_test.c

trie-test: trie.o trie_test.c
	$(CC) $(CFLAGS) -o trie-test trie.o trie_test.c

stack-test: stack.o stack_test.c
	$(CC) $(CFLAGS) -o stack-test stack.o stack_test.c

all: $(PROG)

AST.o: $(AST_SRCS) trie.o list.o
	$(CC) $(CFLAGS) -c AST.c

trie.o: $(TRIE_SRCS)
	$(CC) $(CFLAGS) -c trie.c

list.o: $(LIST_SRCS)
	$(CC) $(CFLAGS) -c list.c

parser.o: $(EXPR_SRCS)
	yacc $(YACCFLAGS) parser.y
	mv y.tab.c parser.c
	$(CC) $(CFLAGS) -c parser.c

tarball:
	mkdir tiny-c
	tar zcvf tiny-c.tar.gz $(SRCS) list_test.c trie_test.c Makefile LICENSE.md README.md
	rm -r tiny-c

clean:
	rm -rf *.dSYM/
	rm -f *.o parser.c *~ $(PROG)
