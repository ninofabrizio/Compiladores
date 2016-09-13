#include "monga.h"

int main( int argc, char *argv[]) {
	
    ++argv, --argc;  /* skip over program name */
    
	int token;
	FILE *file;
	
	// just tests...
	//char _ = 97;
	//printf("%c", _);

    if (argc > 0) {

        file = fopen(argv[0], "r");
        setInput(file);
    }
    
    else
        file = stdin;
    
	while( token = yylex() ) {

		if(token != -1) {
			printf("\ntoken: %d\n", token);

			if(token == 34 || token == 35)
				printf("\nString: %s\n", yyvar.s);
		}

		else
			break;
	}
}