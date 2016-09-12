#include "monga.h"

int main( int argc, char *argv[]) {
	
    ++argv, --argc;  /* skip over program name */
    
	int token;
	FILE *file;
	
	// just tests...
	//char _[] = "";
	//printf("%s a", _);

    if (argc > 0) {

        file = fopen(argv[0], "r");
        setOutput(file);
    }
    
    else
        file = stdin;
    
	while( token = yylex() )
		printf("\ntoken: %d\n", token);
}