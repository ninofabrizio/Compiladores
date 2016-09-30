#include "monga.h"

int yyparse();


int main( int argc, char *argv[] ) {
	
    ++argv, --argc;  // skip over program name
    int retParse;
	
	
	//int token;
	FILE *file;

    if (argc > 0) {
        file = fopen(argv[0], "r");
        setInput(file);
    } else
        file = stdin;
	
	retParse = yyparse();
	
	fclose(file);

	return retParse;
}