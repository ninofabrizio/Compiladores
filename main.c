#include "monga.tab.h"
#include "monga.h"

int yyparse();

int main( int argc, char *argv[] ) {
	
    ++argv, --argc;  // skip over program name
    
	//int token;
	FILE *file;

    if (argc > 0) {

        file = fopen(argv[0], "r");
        setInput(file);
    }
    
    else
        file = stdin;
    
    printf("\n%d\n", yyparse());

	/*while( (token = yylex()) ) {

		if(token != -1) {

			printf("Token: %d\t", token);

			if((token >= 2) && (token <= 18))
				printf("%c\n", yyvar.i);
			else if((token >= 19) && (token <= 35))
				printf("%s$\n", yyvar.s);
			else if(token == 36)
				printf("%d\n", yyvar.i);
			else if(token == 37)
				printf("%x\n", yyvar.i);
			else if(token == 1)
				printf("%f\n", yyvar.f);
			else {
				printf("Error: token returned to main not acceptable\n");
				return 1;
			}
		}

		else
			return 1;
	}*/

	fclose(file);

	return 0;
}