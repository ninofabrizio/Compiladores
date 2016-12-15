#include "monga.h"
#include "ast.h"
#include "single_table.h"
#include "type.h"
#include "printAST.h"
#include "geracod.h"


int yyparse ();
void print_tree ( AST_Node *a, int tabIndex );
void print_single_table ( Stack *mySingleTable );
void build_single_table ( AST_Node *a );
Stack* single_table_create ();
id_entry* single_table_find_current_scope ( Stack *single_table, const char *elemFound );
void type_tree ( AST_Node *a );
void initialBuffer ();
void setFilename ( char *name );


AST_Node *AST_Root;
Stack *single_table;
const char* buffer[MAX_TAM];

int main( int argc, char *argv[] ) {
	
    ++argv, --argc;  // skip over program name
    int retParse;
	
	FILE *file;

    if (argc > 0) {
        file = fopen(argv[0], "r");
        setInput(file);
    } else
        file = stdin;
	
	retParse = yyparse();

	single_table = single_table_create();
	build_single_table(AST_Root);

	type_tree(AST_Root);
	
	initialBuffer();
	setFilename(argv[0]);
	geraCodigo(AST_Root);

	//print_tree(AST_Root, 0);

	fclose(file);

	return retParse;
}