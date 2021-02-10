all:
	bison -d parser/parser.y
	flex parser/parser.lex
	gcc *.c debug/debug.c -g -o machine
