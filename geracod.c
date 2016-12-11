#include "geracod.h"
#include <stdlib.h>
#include <string.h>

static void geracod_def_var ( DefVar *a );
static void geracod_stat ( AST_Node *a );
static void geracod_def ( AST_Node *root );
static void geracod_var ( AST_Node *a );
static void geracod_exp ( AST_Node *a );
static void geracod_stat ( AST_Node *a );
static void geracod_param ( Param *param );
static void geracod_call ( Call *funcCall );
void printBuffer ();


void initialBuffer () {
	
	int i;
	
	for ( i = 0; i < MAX_TAM; i++ )
		buffer[i] = NULL;
}


int getIndexBuffer () {
	
	int i;

	for ( i = 0; i < MAX_TAM; i++ )
		if ( buffer [i] == NULL)
			break;

	if ( i == MAX_TAM ) {
		printf("LLVM CODE GENERATOR Buffer limit was reached");
		exit(0);
	}

	return i;
}


const char* getBitsToNumber ( AST_Node *a ) {

	if ( a -> nodeType == TYPE_INT )
		return "i32";

	else if ( a -> nodeType == TYPE_FLOAT )
		return "f32";

	else
		return "i8";
}


void printBuffer () {

	FILE *file = fopen(fileName, "a");
	int i;

	if (file == NULL) {
	    printf("LLVM CODE GENERATOR Error opening file %s!\n", fileName);
	    exit(1);
	}

	for ( i = 0; buffer[i] != NULL && i < MAX_TAM; i++ )
		fprintf(file, "%s", buffer[i]);
	
	fclose(file);
}


void setFilename( char *name ) {

	int i, j;

	strcpy(fileName, name);

	for(i = 0; fileName[i] != '\0'; i++) {
		if(fileName[i] == '.')
			j = i;
	}

	if(j > 47) {
		printf("LLVM CODE GENERATOR file name is too big (it has to be less than 50 characters)");
		exit(1);
	}

	fileName[j+1] = 'l';
	fileName[j+2] = 'l';
	fileName[j+3] = '\0';

	// To "clear" a file of that name, in case it exists
	fclose(fopen(fileName, "w"));
}


void geraCodigo ( AST_Node *root ) {

 	if( root != NULL )
	   switch( root -> node ) {
			case DEF:  geracod_def(root); break;
			case VAR:  geracod_var(root); break;
			case EXPR: geracod_exp(root); break;
			case STAT: geracod_stat(root); break;
			default: exit(0);
	 	}

	printBuffer();
}


static void geracod_def_var ( DefVar *a ) {





}


static void geracod_stat ( AST_Node *a ) { 

	if(a != NULL) {

		if( a -> nodeType == STAT_WHILE ) {

		}
		else if( a -> nodeType == STAT_IF ) {

		}
		else if( a -> nodeType == STAT_IFELSE ) {

		}
		else if( a -> nodeType == STAT_ASSIGN ) {

		}
		else if( a -> nodeType == STAT_RETURN ) {

			buffer[getIndexBuffer()] =  " ret ";

			geracod_exp(a -> nodeStruct.stat -> u.returnExp00Node);
		}
		else if( a -> nodeType == STAT_FUNC_CALL ) {

		}
	}
}


static void  geracod_var ( AST_Node *a ){ }


static void geracod_exp ( AST_Node *a ){ 

	if( a != NULL ) {

		if ( a -> nodeType == EXPR_VAR ) {
			
			geracod_var(a -> nodeStruct.exp -> u.varNode);

		}
		else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			geracod_call(a -> nodeStruct.exp -> u.functionCall);

			/*if(a -> nodeStruct.exp -> u.functionCall -> linkedFuncNode -> nodeStruct.def -> u.func.tagReturnType == 1)
				temp = getTypingFromType(a -> nodeStruct.exp -> u.functionCall -> linkedFuncNode -> nodeStruct.def -> u.func.ret.dataTypeNode);
			else
				temp = createTypingValue(VOID);*/
		}
		else if ( a -> nodeType == EXPR_NEW ) {

			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_INT || a -> nodeType == EXPR_HEXA || a -> nodeType == EXPR_CHAR ) {

			char expValue[20];
			int n = sprintf(expValue, "i32 %d\n", a -> nodeStruct.exp -> u.ki);
			buffer[getIndexBuffer()] = expValue;

			printBuffer();
			initialBuffer();
		}
		else if ( a -> nodeType == EXPR_FLOAT ) {

			
		}
		else if ( a -> nodeType == EXPR_LIT ) {

			
		}
		else if ( a -> nodeType == EXPR_OR ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_AND ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_EQUAL ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_LEEQ ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_GREQ ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_GREATER ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_LESS ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_NOEQ ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_ADD ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_MIN ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_MUL ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_DIV ) {
			
			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_NOT ) {
			
			geracod_exp(a->left);
		}
		else if ( a -> nodeType == EXPR_NEG ) {

			geracod_exp(a->left);
		}
	}
}
	

static void geracod_def ( AST_Node *root ) {
	
	if( root != NULL ) {
								
		if( root -> nodeType == DEF_FUNC ) {
				
			char labelValue[10];
			int n;

			// We have to keep the attribution or trash will be added to the vector...
			n = sprintf(labelValue, "; L%d", currentLabel);
			
			currentLabel++;
			buffer[getIndexBuffer()] =  labelValue;
			buffer[getIndexBuffer()] = ":\ndefine ";
				
			if ( root -> nodeStruct.def -> u.func.tagReturnType == 0 )
				buffer[getIndexBuffer()] =  "void";
			else
				buffer[getIndexBuffer()] = getBitsToNumber( root -> nodeStruct.def -> u.func.ret.dataTypeNode );
				
			buffer[getIndexBuffer()] = " @";
			buffer[getIndexBuffer()] = root -> nodeStruct.def -> u.func.funcName;
			buffer[getIndexBuffer()] = "(";

			if(root -> nodeStruct.def -> u.func.param != NULL)
				geracod_param(root -> nodeStruct.def -> u.func.param);

			buffer[getIndexBuffer()] = ") {\n";
			
			printBuffer();
			initialBuffer();

			n = sprintf(labelValue, "L%d:\n", currentLabel);
			currentLabel++;
			buffer[getIndexBuffer()] =  labelValue;
				
			geracod_def_var(root -> nodeStruct.def -> u.defVar);
			geracod_stat(root -> nodeStruct.def -> u.func.block);

			if(root -> nodeStruct.def -> u.func.tagReturnType == 0 && root -> nodeStruct.def -> u.func.block == NULL)
				buffer[getIndexBuffer()] =  " ret void\n";

			buffer[getIndexBuffer()] = "}";
		}
		else if( root -> nodeType == DEF_VAR ) {


		}
	}
}


static void geracod_param ( Param *param ) {

	geracod_def_var(param -> var);

	if(param -> proxParam != NULL) {
		buffer[getIndexBuffer()] = ", ";
		geracod_param(param -> proxParam);
	}
}

static void geracod_call ( Call *funcCall ) {




}