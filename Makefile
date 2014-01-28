PROG := tiny-c stack-test

AST_SRCS := AST.h AST.c
STACK_SRCS := stack.h stack.c
EXPR_SRCS := parser.y lexer.c interpreter.c
SRCS := $(AST_SRCS) $(STACK_SRCS) $(EXPR_SRCS)

CC := clang
CFLAGS=-Wall -std=c99 -g -Icstl

tiny-c: parser.o AST.o stack.o interpreter.c
	$(CC) $(CFLAGS) -o tiny-c parser.o AST.o stack.o interpreter.c

test-components: stack-test

stack-test: stack.o stack_test.c
	$(CC) $(CFLAGS) -o stack-test stack.o stack_test.c

all: $(PROG)

AST.o: $(AST_SRCS)
	$(CC) $(CFLAGS) -c AST.c

stack.o: $(STACK_SRCS)
	$(CC) $(CFLAGS) -c stack.c

parser.o: $(EXPR_SRCS)
	yacc $(YACCFLAGS) parser.y
	mv y.tab.c parser.c
	$(CC) $(CFLAGS) -c parser.c

tarball:
	mkdir tiny-c
	tar zcvf tiny-c.tar.gz $(SRCS) stack_test.c Makefile LICENSE.md README.md
	rm -r tiny-c

clean:
	rm -rf *.dSYM/
	rm -f *.o parser.c *~ $(PROG)
