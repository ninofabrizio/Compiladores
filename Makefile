  # compiler flags:
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -std=c99 -O2

  # the build target executable:

monga.c: monga.l
	lex -t monga.l > monga.c
	  
monga.o: monga.c
	gcc -o $(CLFLAGS) monga.o -c monga.c
  
ast.o: ast.c
	gcc -o $(CLFLAGS) ast.o -c ast.c		

monga.tab.c: monga.y
	bison -d monga.y

monga.tab.o: monga.tab.c
	gcc -o $(CLFLAGS) monga.tab.o -c monga.tab.c	

stack.o: stack.c
	gcc -o $(CLFLAGS) stack.o -c stack.c
	
symtab.o: symtab.c
	gcc -o $(CLFLAGS) symtab.o -c symtab.c

single_table.o: single_table.c
	gcc -o $(CLFLAGS) single_table.o -c single_table.c

main.o: main.c
	gcc -o $(CLFLAGS) main.o -c main.c
  
tester: monga.o main.o monga.tab.o ast.o symtab.o stack.o single_table.o
	gcc -o tester main.o monga.o ast.o monga.tab.o symtab.o stack.o single_table.o -ll
  
automatic: automatic.sh
	sh automatic.sh

compile: monga.tab.c monga.tab.o monga.c monga.o ast.o main.o tester

test: automatic

clear:
	rm -f *.o monga.c monga.tab.c monga.tab.h tester
