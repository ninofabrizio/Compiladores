#include "geracod.h"
#include<stdlib.h>

static void geracod_def_var (DefVar *n);
static void geracod_stat (AST_Node *n);
static void geracod_def (AST_Node * root);
static void  geracod_var(AST_Node *n);
static void geracod_exp(AST_Node *n);
static void geracod_stat(AST_Node *n);	
void printBuffer( );

void initialBuffer(  ) {
	
	int i;
	
	for ( i = 0; i < MAX_TAM; i++ )
		buffer[i] = NULL;	
}


void geraCodigo(AST_Node * root) {
	
 	if( root != NULL )	   
	   switch( root -> node ) {
			case DEF:  geracod_def(root); break;
			case VAR:  geracod_var(root); break;
			case EXPR: geracod_exp(root); break;
			case STAT: geracod_stat(root); break;	   
			default: exit(0);					
	 	}
	printBuffer( );
}

int getIndexBuffer () {
	
	int i;
	
	for ( i = 0; i < MAX_TAM; i++ ) 	
		if ( buffer [i] == NULL)
			break;
		
	if ( i == MAX_TAM ) {			
		printf("Buffer limit was reached");
		exit(0);			
	}
	
	return i;
}

const char* getBitsToNumber( AST_Node *n  ) {
	
	if ( n -> nodeType == TYPE_INT )
		return "i32";
		
	else if ( n -> nodeType == TYPE_FLOAT )
		return "f32";
			
	else
		return "i8";	
}

static void geracod_def_var (DefVar *n) { 





}



static void geracod_stat (AST_Node *n) { 





}


static void  geracod_var(AST_Node *n){ }
static void geracod_exp(AST_Node *n){ }
	

void printBuffer( ) {
	
	FILE *file = fopen("file.ll", "w");
	int i;
	
	if (file == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	for ( i = 0; buffer[i] != NULL && i < MAX_TAM; i++ )	
		fprintf(file, "%s", buffer[i]);
		
	
	
	fclose(file);
		
}


static void geracod_def (AST_Node * root) {
	
	if( root != NULL ) {
								
		if( root -> nodeType == DEF_FUNC ) {
											
				buffer[getIndexBuffer()] = "; L";
				buffer[getIndexBuffer()] = "1" ; // implementar atoi
				currentLabel++;
				buffer[getIndexBuffer()] = ":\ndefine ";
				
				if ( root -> nodeStruct.def -> u.func.tagReturnType == 0 )
					buffer[getIndexBuffer()] =  "";
				
				else
					buffer[getIndexBuffer()] = getBitsToNumber( root -> nodeStruct.def -> u.func.ret.dataTypeNode );
				
				buffer[getIndexBuffer()] = "@";
				buffer[getIndexBuffer()] = root -> nodeStruct.def -> u.func.funcName;
				buffer[getIndexBuffer()] = "() {\n  "; // so vale para funcao sem parametros
				
				geracod_def_var(root -> nodeStruct.def -> u.defVar);
				geracod_stat(root -> nodeStruct.def -> u.func.block);
											
		}				
	}
	
}
