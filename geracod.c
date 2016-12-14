#include "geracod.h"
#include <stdlib.h>
#include <string.h>

static void geracod_def_var ( DefVar *a );
static void geracod_stat ( AST_Node *a );
static void geracod_def ( AST_Node *root );
static void geracod_var ( AST_Node *a );
static void geracod_exp ( AST_Node *a );
static void geracod_param ( Param *param );
static void geracod_call ( Call *funcCall );
static void geracod_block ( AST_Node *a );

typedef struct parameterVarList parameterVarList;

struct parameterVarList {

	char tempName[50];
	parameterVarList *nextParameterVarNode;
};

parameterVarList paramList;

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
	else //if ( a -> nodeType == TYPE_CHAR )
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


const char * getTyping ( Typing *typing ) {

	if(typing -> typeKind == INTEGER)
		return "i32";
	else if(typing -> typeKind == FLOAT)
		return "f32";
	else if(typing -> typeKind == VOID)
		return "void";
	else if(typing -> typeKind == ARRAY) {

		printf("\nLLVM: TYPING ARRAY NOT IMPLEMENTED!\n");

	}
	else if(typing -> typeKind == STRING_TYPE) {
		
		printf("\nLLVM: TYPING STRING_TYPE NOT IMPLEMENTED!\n");

	}

	return NULL;
}


void geracod_varDef ( AST_Node *a ) {

	if(a != NULL) {
			
		buffer[getIndexBuffer()] = " ";

		geracod_var(a);

		buffer[getIndexBuffer()] = " = alloca ";
		buffer[getIndexBuffer()] = getTyping(a -> nodeStruct.var -> typing);
		buffer[getIndexBuffer()] = "\n";

		printBuffer();
		initialBuffer();

		geracod_varDef(a -> nodeStruct.var -> nextVarNode);
	}
}


void setTemporaryInit () {

	char value[50];

	int n = sprintf(lastTemp, "%%myt%d", countTemp);
	countTemp++;
	strcpy(value, " ");
	strcat(value, lastTemp);
	strcat(value, " =");
	buffer[getIndexBuffer()] = value;

	printBuffer();
	initialBuffer();
}


void allocaParams ( Param *p ) {

	if(p != NULL) {

		buffer[getIndexBuffer()] = " ";
		
		geracod_var(p -> var -> varListNode);
		
		buffer[getIndexBuffer()] = ".param = alloca ";
		buffer[getIndexBuffer()] = getTyping(p -> var -> varListNode -> nodeStruct.var -> typing);
		buffer[getIndexBuffer()] = "\n store ";
		buffer[getIndexBuffer()] = getTyping(p -> var -> varListNode -> nodeStruct.var -> typing);
		buffer[getIndexBuffer()] = " ";

		geracod_var(p -> var -> varListNode);

		buffer[getIndexBuffer()] = ", ";
		buffer[getIndexBuffer()] = getTyping(p -> var -> varListNode -> nodeStruct.var -> typing);
		buffer[getIndexBuffer()] = "* ";

		geracod_var(p -> var -> varListNode);
		
		buffer[getIndexBuffer()] = ".param\n";

		printBuffer();
		initialBuffer();

		allocaParams(p -> proxParam);
	}
}


void beginLoad ( Typing *typing ) {

	setTemporaryInit();

	buffer[getIndexBuffer()] = " load ";
	buffer[getIndexBuffer()] = getTyping(typing);
	buffer[getIndexBuffer()] = ", ";
	buffer[getIndexBuffer()] = getTyping(typing);
	buffer[getIndexBuffer()] = "* ";
}


void setParameters ( Call *funcCall ) {

	parameterVarList temp1;
	parameterVarList *temp2 = NULL;
	AST_Node *expNode = funcCall -> expressionNode;

	paramList.nextParameterVarNode = NULL;

	while(expNode != NULL) {

		if(expNode -> nodeType != EXPR_INT && expNode -> nodeType != EXPR_HEXA && expNode -> nodeType != EXPR_NEG
			&& expNode -> nodeType != EXPR_CHAR && expNode -> nodeType != EXPR_FLOAT) {

			if(expNode -> nodeType == EXPR_VAR) {
				
				beginLoad(expNode -> nodeStruct.exp -> typing);

				if(paramList.nextParameterVarNode == NULL) {

					strcpy(paramList.tempName, lastTemp);
					paramList.nextParameterVarNode = &paramList;

					temp2 = &paramList;
				}	
				else {
				
					strcpy(temp1.tempName, lastTemp);
					temp1.nextParameterVarNode = NULL;

					temp2 -> nextParameterVarNode = &temp1;
					temp2 = &temp1;
				}

				geracod_exp(expNode);
			}
			else {

				geracod_exp(expNode);

				if(paramList.nextParameterVarNode == NULL) {

					strcpy(paramList.tempName, lastTemp);
					paramList.nextParameterVarNode = &paramList;

						temp2 = &paramList;
				}
				else {
				
					strcpy(temp1.tempName, lastTemp);
					temp1.nextParameterVarNode = NULL;

					temp2 -> nextParameterVarNode = &temp1;
					temp2 = &temp1;
				}
			}

			buffer[getIndexBuffer()] = "\n";

			printBuffer();
			initialBuffer();
		}

		expNode = expNode -> nodeStruct.exp -> nextExpNode;
	}
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
	initialBuffer();
}


static void geracod_def_var ( DefVar *a ) {

	buffer[getIndexBuffer()] = getTyping(a -> varListNode -> nodeStruct.var -> typing);
	buffer[getIndexBuffer()] = " ";

	geracod_var(a -> varListNode);
}


static void geracod_stat ( AST_Node *a ) { 

	if(a != NULL) {

		char statValue[50];

		if( a -> nodeType == STAT_WHILE ) {

			printf("\nLLVM: STAT_WHILE NOT IMPLEMENTED!\n");
		}
		else if( a -> nodeType == STAT_IF ) {

			printf("\nLLVM: STAT_IF NOT IMPLEMENTED!\n");
		}
		else if( a -> nodeType == STAT_IFELSE ) {

			printf("\nLLVM: STAT_IFELSE NOT IMPLEMENTED!\n");
		}
		else if( a -> nodeType == STAT_ASSIGN ) {

			if(a -> nodeStruct.stat -> u.assign.exp00Node -> nodeType != EXPR_INT && a -> nodeStruct.stat -> u.assign.exp00Node -> nodeType != EXPR_HEXA
				&& a -> nodeStruct.stat -> u.assign.exp00Node -> nodeType != EXPR_CHAR && a -> nodeStruct.stat -> u.assign.exp00Node -> nodeType != EXPR_FLOAT) {

				if(a -> nodeStruct.stat -> u.assign.exp00Node -> nodeType == EXPR_VAR)
					beginLoad(a -> nodeStruct.stat -> u.assign.exp00Node -> nodeStruct.exp -> typing);

				geracod_exp(a -> nodeStruct.stat -> u.assign.exp00Node);

				buffer[getIndexBuffer()] =  "\n store ";
				buffer[getIndexBuffer()] = getTyping(a -> nodeStruct.stat -> u.assign.exp00Node -> nodeStruct.exp -> typing);
				buffer[getIndexBuffer()] =  " ";
				buffer[getIndexBuffer()] =  lastTemp;
			}
			else {

				buffer[getIndexBuffer()] =  " store ";
				buffer[getIndexBuffer()] = getTyping(a -> nodeStruct.stat -> u.assign.exp00Node -> nodeStruct.exp -> typing);
				buffer[getIndexBuffer()] =  " ";

				geracod_exp(a -> nodeStruct.stat -> u.assign.exp00Node);
			}

			buffer[getIndexBuffer()] =  ", ";
			buffer[getIndexBuffer()] = getTyping(a -> nodeStruct.stat -> u.assign.varNode -> nodeStruct.var -> typing);
			buffer[getIndexBuffer()] =  "* ";

			geracod_var(a -> nodeStruct.stat -> u.assign.varNode);

			buffer[getIndexBuffer()] =  "\n";
		}
		else if( a -> nodeType == STAT_RETURN ) {

			if(a -> nodeStruct.stat -> u.returnExp00Node != NULL && (a -> nodeStruct.stat -> u.returnExp00Node -> nodeType == EXPR_INT
				|| a -> nodeStruct.stat -> u.returnExp00Node -> nodeType == EXPR_HEXA || a -> nodeStruct.stat -> u.returnExp00Node -> nodeType == EXPR_CHAR
				|| a -> nodeStruct.stat -> u.returnExp00Node -> nodeType == EXPR_FLOAT)) {

				buffer[getIndexBuffer()] =  " ret ";
				buffer[getIndexBuffer()] = getTyping(a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> typing);
				buffer[getIndexBuffer()] =  " ";

				geracod_exp(a -> nodeStruct.stat -> u.returnExp00Node);
			}
			else if(a -> nodeStruct.stat -> u.returnExp00Node != NULL && a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> typing -> typeKind != VOID) {

				if(a -> nodeStruct.stat -> u.returnExp00Node -> nodeType == EXPR_VAR) {

					beginLoad(a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> typing);

					geracod_var(a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> u.varNode);		
				}
				else
					geracod_exp(a -> nodeStruct.stat -> u.returnExp00Node);

				buffer[getIndexBuffer()] =  "\n ret ";
				buffer[getIndexBuffer()] = getTyping(a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> typing);
				buffer[getIndexBuffer()] = " ";
				buffer[getIndexBuffer()] =  lastTemp;
			}
			else {

				if(a -> nodeStruct.stat -> u.returnExp00Node != NULL && a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> typing -> typeKind == VOID) {
					geracod_exp(a -> nodeStruct.stat -> u.returnExp00Node);
					buffer[getIndexBuffer()] =  "\n";
				}

				buffer[getIndexBuffer()] =  " ret void";
			}
		}
		else if( a -> nodeType == STAT_FUNC_CALL ) {

			geracod_call(a -> nodeStruct.stat -> u.callFunc);

			buffer[getIndexBuffer()] =  "\n";
		}

		printBuffer();
		initialBuffer();

		geracod_stat(a->left);
	}
}


static void  geracod_var ( AST_Node *a ){ 

	if(a != NULL) {

		char varValue[50];

		if(a -> nodeType == VAR_UNIQUE) {

			strcpy(varValue, "%");
			strcat(varValue, a -> nodeStruct.var -> varName);

			if(a -> nodeStruct.var -> linkedVarNode != NULL && a -> nodeStruct.var -> linkedVarNode -> node == VAR)
				strcat(varValue, ".param");

			buffer[getIndexBuffer()] = varValue;
		}
		else if(a -> nodeType == VAR_INDEXED) {

			printf("\nLLVM: VAR_INDEXED NOT IMPLEMENTED!\n");
		}

		printBuffer();
		initialBuffer();
	}
}


static void geracod_exp ( AST_Node *a ){ 

	if( a != NULL ) {

		char expValue[50];

		char temp1[50];
		char temp2[50];

		if ( a -> nodeType == EXPR_VAR ) {
			
			geracod_var(a -> nodeStruct.exp -> u.varNode);
		}
		else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			setParameters(a -> nodeStruct.exp -> u.functionCall);

			if( a -> nodeStruct.exp -> typing -> typeKind != VOID) {
				setTemporaryInit();
				printBuffer();
				initialBuffer();
			}

			geracod_call(a -> nodeStruct.exp -> u.functionCall);
		}
		else if ( a -> nodeType == EXPR_NEW ) {

			printf("\nLLVM: EXPR_NEW NOT IMPLEMENTED!\n");

			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_INT || a -> nodeType == EXPR_HEXA || a -> nodeType == EXPR_CHAR ) {

			int n = sprintf(expValue, "%d", a -> nodeStruct.exp -> u.ki);
			buffer[getIndexBuffer()] = expValue;
		}
		else if ( a -> nodeType == EXPR_FLOAT ) {

			printf("\nLLVM: EXPR_FLOAT NOT IMPLEMENTED!\n");
		}
		else if ( a -> nodeType == EXPR_LIT ) {

			printf("\nLLVM: EXPR_LIT NOT IMPLEMENTED!\n");
		}
		else if ( a -> nodeType == EXPR_OR ) {
			
			printf("\nLLVM: EXPR_OR NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_AND ) {
			
			printf("\nLLVM: EXPR_AND NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_EQUAL ) {
			
			printf("\nLLVM: EXPR_EQUAL NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_LEEQ ) {
			
			printf("\nLLVM: EXPR_LEEQ NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_GREQ ) {
			
			printf("\nLLVM: EXPR_GREQ NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_GREATER ) {
			
			printf("\nLLVM: EXPR_GREATER NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_LESS ) {
			
			printf("\nLLVM: EXPR_LESS NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_NOEQ ) {
			
			printf("\nLLVM: EXPR_NOEQ NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
			geracod_exp(a->right);
		}
		else if ( a -> nodeType == EXPR_ADD || a -> nodeType == EXPR_MIN
				|| a -> nodeType == EXPR_MUL || a -> nodeType == EXPR_DIV ) {
			
			if(a -> left -> nodeType == EXPR_VAR) {
				
				beginLoad(a -> left -> nodeStruct.exp -> typing);

				geracod_exp(a->left);
				//geracod_var(a -> left -> nodeStruct.exp -> u.varNode);

				buffer[getIndexBuffer()] = "\n";

				strcpy(temp1, lastTemp);
			}
			else if(a -> left -> nodeType == EXPR_FUNC_CALL) {

				geracod_exp(a -> left);

				strcpy(temp1, lastTemp);
				buffer[getIndexBuffer()] = "\n";
			}

			if(a -> right -> nodeType == EXPR_VAR) {
				
				beginLoad(a -> right -> nodeStruct.exp -> typing);

				geracod_exp(a->right);
				//geracod_var(a -> right -> nodeStruct.exp -> u.varNode);

				buffer[getIndexBuffer()] = "\n";

				strcpy(temp2, lastTemp);
			}
			else if(a -> right -> nodeType == EXPR_FUNC_CALL) {

				geracod_exp(a -> right);

				strcpy(temp2, lastTemp);
				buffer[getIndexBuffer()] = "\n";
			}

			setTemporaryInit();

			if(a -> nodeStruct.exp -> typing -> typeKind == INTEGER) {
				if(a -> nodeType == EXPR_ADD)
					buffer[getIndexBuffer()] = " add i32 ";
				else if(a -> nodeType == EXPR_MIN)
					buffer[getIndexBuffer()] = " sub i32 ";
				else if(a -> nodeType == EXPR_MUL)
					buffer[getIndexBuffer()] = " mul i32 ";
				else if(a -> nodeType == EXPR_DIV)
					buffer[getIndexBuffer()] = " sdiv i32 "; // signed division
			}
			else if(a -> nodeStruct.exp -> typing -> typeKind == FLOAT) { // not sure if correct
				if(a -> nodeType == EXPR_ADD)
					buffer[getIndexBuffer()] = " fadd float ";
				else if(a -> nodeType == EXPR_MIN)
					buffer[getIndexBuffer()] = " fsub float ";
				else if(a -> nodeType == EXPR_MUL)
					buffer[getIndexBuffer()] = " fmul float ";
				else if(a -> nodeType == EXPR_DIV)
					buffer[getIndexBuffer()] = " fdiv float ";
			}

			if(a -> left -> nodeType != EXPR_VAR && a -> left -> nodeType != EXPR_FUNC_CALL)
				geracod_exp(a->left);
			else
				buffer[getIndexBuffer()] = temp1;

			buffer[getIndexBuffer()] = ", ";

			if(a -> right -> nodeType != EXPR_VAR && a -> right -> nodeType != EXPR_FUNC_CALL)
				geracod_exp(a->right);
			else
				buffer[getIndexBuffer()] = temp2;
		}
		else if ( a -> nodeType == EXPR_NOT ) {
			
			printf("\nLLVM: EXPR_NOT NOT IMPLEMENTED!\n");

			geracod_exp(a->left);
		}
		else if ( a -> nodeType == EXPR_NEG ) {

			geracod_exp(a->left);
		}

		printBuffer();
		initialBuffer();
	}
}
	

static void geracod_def ( AST_Node *a ) {
	
	if( a != NULL ) {
								
		if( a -> nodeType == DEF_FUNC ) {
				
			char labelValue[10];
			int n;

			buffer[getIndexBuffer()] = "\ndefine ";
				
			if ( a -> nodeStruct.def -> u.func.tagReturnType == 0 )
				buffer[getIndexBuffer()] =  "void";
			else
				buffer[getIndexBuffer()] = getBitsToNumber( a -> nodeStruct.def -> u.func.ret.dataTypeNode );
				
			buffer[getIndexBuffer()] = " @";
			buffer[getIndexBuffer()] = a -> nodeStruct.def -> u.func.funcName;
			buffer[getIndexBuffer()] = "(";

			geracod_param(a -> nodeStruct.def -> u.func.param);

			buffer[getIndexBuffer()] = ") {\n";
			
			printBuffer();
			initialBuffer();

			n = sprintf(labelValue, "L%d:\n", currentLabel);
			currentLabel++;
			buffer[getIndexBuffer()] =  labelValue;

			allocaParams(a -> nodeStruct.def -> u.func.param);

			geracod_block(a -> nodeStruct.def -> u.func.block);

			if(a -> nodeStruct.def -> u.func.tagReturnType == 0 && a -> nodeStruct.def -> u.func.block == NULL)
				buffer[getIndexBuffer()] =  " ret void";

			buffer[getIndexBuffer()] = "\n}\n";

			currentLabel = 0;
			countTemp = 0;
		}
		else if( a -> nodeType == DEF_VAR ) {

			geracod_varDef(a -> nodeStruct.def -> u.defVar -> varListNode);
		}

		geraCodigo(a -> left);
	}
}


static void geracod_block ( AST_Node *a ) {

	if( a != NULL ) {
		
		if ( a -> nodeType == DEF_VAR ) {
			
			geracod_def(a);
			geracod_stat(a->right);
		}
		else if( a -> node == STAT )
			geracod_stat(a);
	}
}

static void geracod_param ( Param *param ) {

	if(param != NULL) {

		geracod_def_var(param -> var);

		if(param -> proxParam != NULL) {
			buffer[getIndexBuffer()] = ", ";
			geracod_param(param -> proxParam);
		}
		else { // this so the last parameter "writes" them all
			printBuffer();
			initialBuffer();
		}
	}
}

static void geracod_call ( Call *funcCall ) {

	AST_Node *expNode = funcCall -> expressionNode;
	parameterVarList *temp = NULL;

	buffer[getIndexBuffer()] = " call ";

	if ( funcCall -> linkedFuncNode -> nodeStruct.def -> u.func.tagReturnType == 0 )
		buffer[getIndexBuffer()] =  "void";
	else
		buffer[getIndexBuffer()] = getBitsToNumber(funcCall -> linkedFuncNode -> nodeStruct.def -> u.func.ret.dataTypeNode);

	buffer[getIndexBuffer()] = " ";

	buffer[getIndexBuffer()] = "@";
	buffer[getIndexBuffer()] = funcCall -> funcName;
	buffer[getIndexBuffer()] = "(";

	temp = &paramList;
	expNode = funcCall -> expressionNode;

	while(expNode != NULL) {

		buffer[getIndexBuffer()] = getTyping(expNode -> nodeStruct.exp -> typing);
		buffer[getIndexBuffer()] = " ";

		if(expNode -> nodeType == EXPR_INT || expNode -> nodeType == EXPR_HEXA || expNode -> nodeType == EXPR_NEG
			|| expNode -> nodeType == EXPR_CHAR || expNode -> nodeType == EXPR_FLOAT)
			geracod_exp(expNode);
		else {

			buffer[getIndexBuffer()] = temp -> tempName;
			temp = temp -> nextParameterVarNode;
		}

		if(expNode -> nodeStruct.exp -> nextExpNode != NULL)
			buffer[getIndexBuffer()] = ", ";

		printBuffer();
		initialBuffer();

		expNode = expNode -> nodeStruct.exp -> nextExpNode;
	}

	buffer[getIndexBuffer()] = ")";

	printBuffer();
	initialBuffer();
}