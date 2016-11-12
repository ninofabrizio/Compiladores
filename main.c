#include "monga.h"
#include "ast.h"
//#include "type.h"
#include "single_table.h"

int yyparse();
AST_Node *AST_Root;
void print_tree(AST_Node *a, int tabIndex);
void build_single_table (AST_Node *a);
Stack *single_table;
Stack* single_table_create (void);
id_entry* single_table_find_current_scope (Stack *single_table, const char *elemFound);


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
	
	// PRIMEIRO COSTURA

	// DEPOIS TIPAGEM
	//type_tree(AST_Root);

	single_table = single_table_create();
	build_single_table(AST_Root);
	
	
	
	print_tree(AST_Root, 0);
	
	fclose(file);
	
	return retParse;
}