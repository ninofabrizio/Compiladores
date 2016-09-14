#include "monga.h"

int main( int argc, char *argv[]) {
	
    ++argv, --argc;  /* skip over program name */
    
	int token;
	FILE *file;

	// just tests...
	//char _ = '.';
	//printf("\a");

    if (argc > 0) {

        file = fopen(argv[0], "r");
        setInput(file);
    }
    
    else
        file = stdin;
    
	while( token = yylex() ) {

		if(token != -1) {
			printf("\ntoken: %d\n", token);

			if(token == 34)
				printf("\nChar: %c\n", yyvar.i);
			else if(token == 35)
				printf("\nString: %s$ of Size: %d\n", yyvar.s, strlen(yyvar.s));
			else if(token == 37)
				printf("\nHexa: %x\n", yyvar.i);
			else
				printf("\ntoken: %d\n", token);
		}

		else
			break;
	}
}