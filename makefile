X = mp

all: $(X)

$(X): lex.yy.c parser.tab.c hashtable.c mp.c 
	gcc -o $(X) lex.yy.c parser.tab.c hashtable.c $(X).c 

lex.yy.c: lexer.l
	flex lexer.l

parser.tab.c: parser.y
	bison -o parser.tab.c -y -d parser.y 

clean:
	$(RM) lex.yy.c parser.tab.c parser.tab.h $(X)

