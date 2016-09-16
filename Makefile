  # compiler flags:
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

  # the build target executable:
monga.c: monga.l
	lex -t monga.l > monga.c
	  
monga.o: monga.c
	gcc -o $(CLFLAGS) monga.o -c monga.c
  
main.o: main.c
	gcc -o $(CLFLAGS) main.o -c main.c
  
tester: monga.o main.o
	gcc -o tester main.o monga.o -ll
  
automatic: automatic.sh
	sh automatic.sh

run: monga.c monga.o main.o tester automatic