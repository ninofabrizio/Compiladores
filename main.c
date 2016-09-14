#include "monga.h"

int main( int argc, char *argv[]) {
	
    ++argv, --argc;  /* skip over program name */
    
	int token;
	FILE *file;
	
    if (argc > 0) {

        file = fopen(argv[0], "r");
        setInput(file);
    }
    
    else
        file = stdin;
    
	while ( token = yylex()  ) {
		
		if(token == 37)
			printf("\nString: %d\n", yyvar.i);
		else
			printf("\ntoken: %d\n", token);
			
	}
	
}