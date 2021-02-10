all:
	bison -d parser/parser.y
	flex parser/parser.lex
	gcc *.c -g -o machine
