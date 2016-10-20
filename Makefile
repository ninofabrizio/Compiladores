  # compiler flags:
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

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

main.o: main.c
	gcc -o $(CLFLAGS) main.o -c main.c
  
tester: monga.o main.o monga.tab.o ast.o
	gcc -o tester main.o monga.o ast.o monga.tab.o -ll
  
automatic: automatic.sh
	sh automatic.sh

run: monga.tab.c monga.tab.o monga.c monga.o ast.o main.o tester automatic