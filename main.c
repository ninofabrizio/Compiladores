#include "monga.h"
#include "ast.h"

int yyparse();
AST_Node *AST_Root;
void print_tree(AST_Node *a);

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
	
	print_tree(AST_Root);
	
	fclose(file);
	
	return retParse;
}