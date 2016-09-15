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
		
		if(token == 1){
			printf("\nString: %f\n", yyvar.f);
			printf("\ntoken: %d\n", token);
		}	
		else
			printf("\ntoken: %d\n", token);
	
	
	}
	
}