#include <string.h>
#include "ast.h"

typedef struct typeList typeList;

static void* myMalloc ( size_t size );
static void type_def ( AST_Node *a );
static void type_block ( AST_Node *a, typeList *retList );
static void type_call ( Call *a );
static void type_stat ( AST_Node *a, typeList *retList );
//static void type_type ( AST_Node *a );
static void type_var ( AST_Node *a );
static void type_exp ( AST_Node *a, typeList *arguments );

static Typing* getTypingFromType ( AST_Node *a );
static int verifySameType ( Typing *type1, Typing *type2 );
static int verifyTypeList ( Typing *type, typeList *types );
static int verifyFuncParameters ( typeList *parameters, typeList *arguments );
static Typing * convertTo ( Typing *type, typeEnum thisType );
static int verifyBoolean ( Typing *type );
static Typing * createTypingValue ( typeEnum type, valueEnum valueType, int intValue, float floatValue, const char *string );
static int passValue ( Typing *type1, Typing *type2 );
static Typing * logicExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind );
static Typing * binaryExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind );
static int notOfType ( typeEnum type, typeEnum comparison );
static void verifyIfLiterals ( Typing *type1, Typing *type2, const char* enumT, int line );

#define new(T) ((T*)myMalloc(sizeof(T)))


// Structure list for a sequence of types (needed for returns inside a func def and func calling parameters)
struct typeList {

	Typing *typing;
	typeList *nextTypingNode;
};


static void*
myMalloc ( size_t size ) {
	
	void* obj = (void *)malloc(size);
	
	if(obj == NULL) { 	
		fprintf(stderr, "\nMEMORY SHORTAGE ERROR\n"); 
		exit(0); 	
	}	
	return obj;
}


// Returns a Typing structure based on what's inside the Type one received
static Typing*
getTypingFromType ( AST_Node *a ) {

	int i;
	Typing *typing = NULL, *temp = NULL;
	const char *type = NULL;

	if(a != NULL) {

		typing = new(Typing);
		type = a -> nodeStruct.type -> baseType;

		if(strcmp(type, "int") == 0 || strcmp(type, "char") == 0)
			typing -> typeKind = INTEGER;
		else if(strcmp(type, "float") == 0)
			typing -> typeKind = FLOAT;

		typing -> type = NONE;

		for(i = 0; i < a -> nodeStruct.type -> arraySequence ; i++) {
			temp = new(Typing);
			temp -> typeKind = ARRAY;
			temp -> type = NONE;

			temp -> nextTyping = typing;
			typing = temp;
		}
	}

	return typing;
}


// Returns 1 if Typings are equal, 0 if not
static int
verifySameType ( Typing *type1, Typing *type2 ) {

	int ret = 0;

	if(type1 -> typeKind == type2 -> typeKind) {
		
		if( type1 -> typeKind == ARRAY && type2 -> typeKind == ARRAY )
			ret = verifySameType(type1 -> nextTyping, type2 -> nextTyping);
		else
			ret = 1;
	}
	
	return ret;
}


// Returns 1 if all types in the list match with typing of comparison, 0 if not
static int
verifyTypeList ( Typing *type, typeList *types ) {

	typeList *temp = NULL;
	int ret = 1;

	if(types -> typing != NULL && type -> typeKind != VOID) {
		while(types != NULL && types -> typing != NULL && ret == 1) {

			ret = verifySameType(type, types -> typing);

			temp = types -> nextTypingNode;
			free(types);
			types = temp;
		}
	}
	else if((types -> typing == NULL && type -> typeKind != VOID)
			|| (types -> typing != NULL && type -> typeKind == VOID))
		ret = 0;

	if(types != NULL) {
		while(types != NULL) {
			temp = types -> nextTypingNode;
			free(types);
			types = temp;
		}
	}
	
	return ret;
}


// Returns 1 if both list are of the same size and each node is of the same type (in order),
// 0 if types are not matching, 2 if more parameters than arguments, 3 if more arguments than parameters
//Obs.: Parameters -> Func Def && Arguments -> Func Call
static int
verifyFuncParameters ( typeList *parameters, typeList *arguments ) {

	typeList *temp = NULL;
	int ret = 1;

	if(parameters != NULL && parameters -> typing != NULL && arguments != NULL && arguments -> typing != NULL)
		while(parameters != NULL && parameters -> typing != NULL && arguments != NULL && arguments -> typing != NULL && ret == 1) {
			ret = verifySameType(parameters -> typing, arguments -> typing);

			temp = parameters -> nextTypingNode;
			free(parameters);
			parameters = temp;
			temp = arguments -> nextTypingNode;
			free(arguments);
			arguments = temp;
		}

	if(ret == 1 && arguments -> typing == NULL && parameters -> typing != NULL)
		ret = 2;
	else if(ret == 1 && parameters -> typing == NULL && arguments -> typing != NULL)
		ret = 3;

	if(parameters != NULL) {
		while(parameters != NULL) {
			temp = parameters -> nextTypingNode;
			free(parameters);
			parameters = temp;
		}
	}
	
	if(arguments != NULL) {
		while(arguments != NULL) {
			temp = arguments -> nextTypingNode;
			free(arguments);
			arguments = temp;
		}
	}

	return ret;
}


// Returns a new Typing structure with a converted value to integer or float
static Typing *
convertTo ( Typing *type, typeEnum thisType ) {

	Typing *temp = type;
	Typing *convertedTyping = new(Typing);

	while(temp -> typeKind == ARRAY)
		temp = temp -> nextTyping;

	if(thisType == INTEGER) {

		convertedTyping -> typeKind = INTEGER;
		convertedTyping -> type = INT_VALUE;

		if(temp -> type == FLOAT_VALUE) {
			if(temp -> typeValue.floatValue >= 0)
        		convertedTyping -> typeValue.intValue = (int) (temp -> typeValue.floatValue + 0.5); 
   			else 
        		convertedTyping -> typeValue.intValue = (int) (temp -> typeValue.floatValue - 0.5);
		}
		else if(temp -> type == STRING)
			convertedTyping -> typeValue.intValue = 1;
	}
	else {

		convertedTyping -> typeKind = FLOAT;
		convertedTyping -> type = FLOAT_VALUE;

		if(temp -> type == INT_VALUE)
			convertedTyping -> typeValue.floatValue = (float) (temp -> typeValue.intValue);
		else if(temp -> type == STRING)
			convertedTyping -> typeValue.floatValue = 1.0;
	}

	return convertedTyping;
}


// Returns 0 if value inside Typing equals 0, 1 if it's different from 0, -1 if the value inside is not integer type
static int
verifyBoolean ( Typing *type ) {

	if(type -> type != INT_VALUE)
		return -1;

	return (type -> typeValue.intValue != 0);
}


// Creates a new Typing structure with it's respective value inside
static Typing *
createTypingValue ( typeEnum typeNum, valueEnum valueType, int intValue, float floatValue, const char *string ) {

	Typing *type = new(Typing);

	type -> typeKind = typeNum;
	type -> type = valueType;
	type -> nextTyping = NULL;

	if(valueType == INT_VALUE)
		type -> typeValue.intValue = intValue;
	else if(valueType == FLOAT_VALUE)
		type -> typeValue.floatValue = floatValue;
	else if(typeNum == STRING_TYPE && valueType == STRING)
		type -> typeValue.string = string;

	return type;
}


// Pass value from type2 to type1, returns 1 if done with no problems, 0 if values inside Typings are not matching or type2 value is empty
static int
passValue ( Typing *type1, Typing *type2 ) {

	if( type2 -> type == NONE || (type1 -> type != NONE && (type1 -> type != type2 -> type)))
		return 0;
	else {
		if(type1 -> type == NONE)
			type1 -> type = type2 -> type;

		if(type1 -> type == INT_VALUE)
			type1 -> typeValue.intValue = type2 -> typeValue.intValue;
		else if(type1 -> type == FLOAT_VALUE)
			type1 -> typeValue.floatValue = type2 -> typeValue.floatValue;
		else if(type1 -> type == STRING)
			type1 -> typeValue.string = type2 -> typeValue.string;
	}

	return 1;
}


// Creates a new Typing holding the result of a logic comparison between one or two Typings from expressions, converts their values if necessary
static Typing *
logicExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind ) {

	Typing *logic = new(Typing), *t1 = NULL, *t2 = NULL;
	logic -> typeKind = INTEGER;
	logic -> type = INT_VALUE;

	if(type2 == NULL && kind == EXPR_NOT)
		logic -> typeValue.intValue = !(type1 -> typeValue.intValue);
	else{
		if(type1 -> typeKind != INT_VALUE)
			t1 = convertTo(type1, INTEGER);
		else
			t1 = type1;
		
		if(type2 -> typeKind != INT_VALUE)
			t2 = convertTo(type2, INTEGER);
		else
			t2 = type2;

		if(kind == EXPR_OR)
			logic -> typeValue.intValue = (t1 -> typeValue.intValue || t2 -> typeValue.intValue);
		else if(kind == EXPR_AND)
			logic -> typeValue.intValue = (t1 -> typeValue.intValue && t2 -> typeValue.intValue);

		if(type1 -> typeKind != INT_VALUE)
			free(t1);
		if(type2 -> typeKind != INT_VALUE)
			free(t2);
	}
	
	return logic;
}


// Creates a new Typing holding the result of a binary operation or comparison between two Typings from expressions, converts their values if necessary
static Typing *
binaryExpressions( Typing *type1, Typing *type2, nodeTypeEnum kind ) {

	Typing *binary = new(Typing), *t1 = NULL, *t2 = NULL;

	if(type1 -> typeKind != type2 -> typeKind && type1 -> typeKind != FLOAT_VALUE)
		t1 = convertTo(type1, FLOAT);
	else
		t1 = type1;
	
	if(type1 -> typeKind != type2 -> typeKind && type2 -> typeKind != FLOAT_VALUE)
		t2 = convertTo(type2, FLOAT);
	else
		t2 = type2;

	if(kind == EXPR_EQUAL)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue == t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue == t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_LEEQ)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue <= t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue <= t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_GREQ)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue >= t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue >= t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_GREATER)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue > t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue > t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_LESS)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue < t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue < t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_NOEQ)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue != t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue != t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_ADD)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue + t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue + t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_MIN)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue - t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue - t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_MUL)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue * t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue * t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}
	else if(kind == EXPR_DIV)
		if(t1 -> typeKind == INTEGER) {
			binary -> typeValue.intValue = (t1 -> typeValue.intValue / t2 -> typeValue.intValue);
			binary -> typeKind = INTEGER;
			binary -> type = INT_VALUE;
		}
		else {
			binary -> typeValue.floatValue = (t1 -> typeValue.floatValue / t2 -> typeValue.floatValue);
			binary -> typeKind = FLOAT;
			binary -> type = FLOAT_VALUE;
		}

	if(type1 -> typeKind != FLOAT_VALUE)
		free(t1);
	if(type2 -> typeKind != FLOAT_VALUE)
		free(t2);
	
	return binary;
}


// Return 0 if typeEnums match, 1 if not
static int
notOfType ( typeEnum type, typeEnum comparison ) {

	return (type != comparison);
}


// Stops the program if one of the expressions' typings in a binary expression is STRING_TYPE
static void
verifyIfLiterals ( Typing *type1, Typing *type2, const char* enumT, int line ) {

	if(type1 -> typeKind == STRING_TYPE || type2 -> typeKind == STRING_TYPE) {

		printf("\n%s ONE OF THE EXPS IS LITERAL\tLINE: %d\n", enumT, line);
		exit(0);
	}
}


// Prints error message for function calling and stops execution
static void
printFuncCallErrorMsg( int ret, Call *a ) {

	if(ret == 0) {
		if(a -> expressionNode == NULL)
			printf("\nFUNC_CALL TYPES NOT MATCHING\tFUNC NAME: %s\n", a -> funcName);
		else
			printf("\nFUNC_CALL TYPES NOT MATCHING\tFUNC NAME: %s\tLINE: %d\n", a -> funcName, a -> expressionNode -> line);
	}
	else if(ret == 2) {
		if(a -> expressionNode == NULL)
			printf("\nFUNC_CALL MISSING ARGUMENTS\tFUNC NAME: %s\n", a -> funcName);
		else
			printf("\nFUNC_CALL MISSING ARGUMENTS\tFUNC NAME: %s\tLINE: %d\n", a -> funcName, a -> expressionNode -> line);
	}
	else if(ret == 3) {
		if(a -> expressionNode == NULL)
			printf("\nFUNC_CALL TOO MANY ARGUMENTS\tFUNC NAME: %s\n", a -> funcName);
		else
			printf("\nFUNC_CALL TOO MANY ARGUMENTS\tFUNC NAME: %s\tLINE: %d\n", a -> funcName, a -> expressionNode -> line);
	}
			
	exit(0);
}


/*static void
type_type ( AST_Node *a ) {
	
	if( a != NULL ) {

		printf("TYPE: ");
		printf("%s", a -> nodeStruct.type -> baseType);
	}	
}*/


static void//			TODO INACABADO
type_exp ( AST_Node *a, typeList *arguments ) {

	int intVal = 0;
	float floatVal = 0.0;
	const char *stringVal = NULL;
	Typing *temp = NULL;
	typeList *tempList = NULL;

	if( a != NULL ) {

		if ( a -> nodeType == EXPR_VAR ) {
			
			type_var(a -> nodeStruct.exp -> u.varNode);

			temp = a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing;

			while(temp -> typeKind == ARRAY)
				temp = temp -> nextTyping;

			if(temp -> type == INT_VALUE)
				intVal = temp -> typeValue.intValue;
			else if(temp -> type == FLOAT_VALUE)
				floatVal = temp -> typeValue.floatValue;
			else if(temp -> type == STRING)
				stringVal = temp -> typeValue.string;

			a -> nodeStruct.exp -> typing = createTypingValue(temp -> typeKind, temp -> type, intVal, floatVal, stringVal);
		}
		else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			type_call(a -> nodeStruct.exp -> u.functionCall);

			//a -> nodeStruct.exp -> typing = createTypingValue( TIPO DADO PELA TABELA? , NONE, intVal, floatVal, stringVal);
		}
		else if ( a -> nodeType == EXPR_NEW ) {
			
			//type_type(a->left);
			type_exp(a->right, NULL);
			
			temp = a -> right -> nodeStruct.exp -> typing;

			if(notOfType(temp -> typeKind, INTEGER)) {

				printf("\nEXPR_NEW EXP IN THE RIGHT IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", temp -> typeKind, a -> line);
				exit(0);
			}

			a -> nodeStruct.exp -> typing = getTypingFromType(a -> left);
		}
		else if ( a -> nodeType == EXPR_INT || a -> nodeType == EXPR_HEXA || a -> nodeType == EXPR_CHAR ) {

			a -> nodeStruct.exp -> typing = createTypingValue(INTEGER, INT_VALUE, a -> nodeStruct.exp -> u.ki, 0.0, NULL);
		}
		else if ( a -> nodeType == EXPR_FLOAT ) {

			a -> nodeStruct.exp -> typing = createTypingValue(FLOAT, FLOAT_VALUE, 0, a -> nodeStruct.exp -> u.kf, NULL);
		}
		else if ( a -> nodeType == EXPR_LIT ) {

			a -> nodeStruct.exp -> typing = createTypingValue(STRING_TYPE, STRING, 0, 0.0, a -> nodeStruct.exp -> u.lit);
		}
		else if ( a -> nodeType == EXPR_OR ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			a -> nodeStruct.exp -> typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_OR);
		}
		else if ( a -> nodeType == EXPR_AND ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			a -> nodeStruct.exp -> typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_AND);
		}
		else if ( a -> nodeType == EXPR_EQUAL ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_EQUAL", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_EQUAL);
		}
		else if ( a -> nodeType == EXPR_LEEQ ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_LEEQ", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_LEEQ);
		}
		else if ( a -> nodeType == EXPR_GREQ ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_GREQ", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_GREQ);
		}
		else if ( a -> nodeType == EXPR_GREATER ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_GREATER", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_GREATER);
		}
		else if ( a -> nodeType == EXPR_LESS ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_LESS", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_LESS);
		}
		else if ( a -> nodeType == EXPR_NOEQ ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_NOEQ", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_NOEQ);
		}
		else if ( a -> nodeType == EXPR_ADD ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_ADD", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_ADD);
		}
		else if ( a -> nodeType == EXPR_MIN ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_MIN", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_MIN);
		}
		else if ( a -> nodeType == EXPR_MUL ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_MUL", a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_MUL);
		}
		else if ( a -> nodeType == EXPR_DIV ) {
			
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, "EXPR_DIV", a -> line);

			if((a -> right -> nodeStruct.exp -> typing -> type == INT_VALUE && a -> right -> nodeStruct.exp -> typing -> typeValue.intValue == 0)
				|| (a -> right -> nodeStruct.exp -> typing -> type == FLOAT_VALUE && a -> right -> nodeStruct.exp -> typing -> typeValue.floatValue == 0.0)) {

				printf("\nEXPR_DIV EXP IN THE DIVIDER IS ZERO\tLINE: %d\n", a -> line);
				exit(0);
			}

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_DIV);
		}
		else if ( a -> nodeType == EXPR_NOT ) {
			
			type_exp(a->left, NULL);

			a -> nodeStruct.exp -> typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, NULL, EXPR_NOT);
		}
		else if ( a -> nodeType == EXPR_NEG ) {

			type_exp(a->left, NULL);

			if(a -> left -> nodeStruct.exp -> typing -> type != INT_VALUE && a -> left -> nodeStruct.exp -> typing -> type != FLOAT_VALUE) {

				printf("\nEXPR_NEG EXP VALUE IS NOT INTEGER OR FLOAT\tLINE: %d\n", a -> line);
				exit(0);
			}

			if( a -> left -> nodeStruct.exp -> typing -> type == INT_VALUE ) {
				
				a -> nodeStruct.exp -> typing = createTypingValue(INTEGER, INT_VALUE, a -> left -> nodeStruct.exp -> typing -> typeValue.intValue, 0.0, NULL);
			}
			else if( a -> left -> nodeStruct.exp -> typing -> type == FLOAT_VALUE ) {
				
				a -> nodeStruct.exp -> typing = createTypingValue(INTEGER, INT_VALUE, 0, a -> left -> nodeStruct.exp -> typing -> typeValue.floatValue, NULL);
			}
		}
		else {

			fprintf(stderr, "\nAST TYPING ERROR(EXP): UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}

		if(a -> nodeStruct.exp -> nextExpNode != NULL && arguments != NULL) {

			tempList = new(typeList);

			if(arguments -> typing != NULL) {
				tempList -> typing = a -> nodeStruct.exp -> typing;
				tempList -> nextTypingNode = arguments;

				arguments = tempList;
			}
			else {
				tempList -> typing = a -> nodeStruct.exp -> nextExpNode -> nodeStruct.exp -> typing;
				tempList -> nextTypingNode = NULL;

				arguments -> typing = a -> nodeStruct.exp -> typing;
				arguments -> nextTypingNode = tempList;
			}
		}
		else if(a -> nodeStruct.exp -> nextExpNode == NULL && arguments != NULL && arguments -> typing == NULL) {
			arguments -> typing = a -> nodeStruct.exp -> typing;
			arguments -> nextTypingNode = NULL;
		}

		/*if(a -> left != NULL && a -> left -> node == EXPR)
			type_exp(a->left, NULL);
		if(a -> right != NULL && a -> right -> node == EXPR)
			type_exp(a->right, NULL);*/

		type_exp(a -> nodeStruct.exp -> nextExpNode, arguments);
	}
}


static void//		TODO INACABADO
type_var( AST_Node *a ) {

	if( a != NULL ) {

		if ( a -> nodeType == VAR_UNIQUE ) {

			//if(a -> nodeStruct.var -> typing == NULL)
			//	a -> nodeStruct.stat -> typing = createTypingValue( TIPO DADO PELA TABELA? , TIPO VALOR DADO PELA TABELA? , VALOR DADO PELA TABELA? , VALOR DADO PELA TABELA? , STRING DADA PELA TABELA? );
		}
		else if ( a -> nodeType == VAR_INDEXED ) {
		
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			if(notOfType(a -> right -> nodeStruct.exp -> typing -> typeKind, INTEGER)) {

				printf("\nVAR_INDEXED EXP IN THE RIGHT IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", a -> right -> nodeStruct.exp -> typing -> typeKind, a -> line);
				exit(0);
			}

			//if(a -> nodeStruct.var -> typing == NULL)
			//	a -> nodeStruct.var -> typing = createTypingValue( TIPO DADO PELA TABELA? , TIPO VALOR DADO PELA TABELA? , VALOR DADO PELA TABELA? , VALOR DADO PELA TABELA? , STRING DADA PELA TABELA? );			
		}
		else {

			fprintf(stderr, "\nAST TYPING ERROR(VAR): UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}

		type_var(a -> nodeStruct.var -> nextVarNode);
		
		type_tree(a->left);
		type_tree(a->right);
	}
}


static void
type_params ( Param *p ) {
	
	Typing *varTyping = NULL;

	if( p != NULL && p -> var -> varListNode -> nodeStruct.var -> typing == NULL) {

		//type_type(p -> var -> dataTypeNode);
		type_var(p -> var -> varListNode);

		if(p -> var -> varListNode -> nodeStruct.var -> typing == NULL)
			p -> var -> varListNode -> nodeStruct.var -> typing = getTypingFromType(p -> var -> dataTypeNode);

		type_params(p -> proxParam);
	}
}


static void
type_stat ( AST_Node *a, typeList *retList ) {
	
	int ret;
	Typing *type = NULL, *t1 = NULL, *t2 = NULL;
	typeList *tempList = NULL;

	if( a != NULL ) {
		
		if( a -> nodeType == STAT_WHILE ) {
			
			type_exp(a -> nodeStruct.stat -> u.whileLoop.exp00Node, NULL);
			type_stat(a -> nodeStruct.stat -> u.whileLoop.commandListNode, retList);

			t1 = a -> nodeStruct.stat -> u.whileLoop.exp00Node -> nodeStruct.exp -> typing;

			if(notOfType(t1 -> typeKind, INTEGER))
				t1 = convertTo(t1, INTEGER);
			
			ret = verifyBoolean(t1);

			if(ret == -1) {

				fprintf(stderr, "\nAST TYPING ERROR(STAT_WHILE): UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}

			type = createTypingValue(INTEGER, INT_VALUE, ret, 0.0, NULL);
			a -> nodeStruct.stat -> typing = type;
		}
		else if ( a -> nodeType == STAT_IF ) {

			type_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, NULL);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.block, retList);

			t1 = a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing;

			if(notOfType(t1 -> typeKind, INTEGER))
				t1 = convertTo(t1, INTEGER);

			ret = verifyBoolean(t1);

			if(ret == -1) {
					
				fprintf(stderr, "\nAST TYPING ERROR(STAT_IF): UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}

			type = createTypingValue(INTEGER, INT_VALUE, ret, 0.0, NULL);
			a -> nodeStruct.stat -> typing = type;
		}
		else if( a -> nodeType == STAT_IFELSE ) {
			
			type_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, NULL);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.block, retList);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo, retList);

			t1 = a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing;

			if(notOfType(t1 -> typeKind, INTEGER))
				t1 = convertTo(t1, INTEGER);

			ret = verifyBoolean(t1);

			if(ret == -1) {
					
				fprintf(stderr, "\nAST TYPING ERROR(STAT_IFELSE): UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}

			type = createTypingValue(INTEGER, INT_VALUE, ret, 0.0, NULL);
			a -> nodeStruct.stat -> typing = type;
		}
		else if( a -> nodeType == STAT_ASSIGN ) {

			type_var(a -> nodeStruct.stat -> u.assign.varNode);
			type_exp(a -> nodeStruct.stat -> u.assign.exp00Node, NULL);

			t1 = a -> nodeStruct.stat -> u.assign.varNode -> nodeStruct.var -> typing;
			t2 = a -> nodeStruct.stat -> u.assign.exp00Node -> nodeStruct.exp -> typing;

			if(verifySameType(t1, t2) == 0) {

				printf("\nSTAT_ASSIGN VAR AND EXP ARE NOT THE SAME TYPE\tVAR ENUM TYPE: %d\tEXP ENUM TYPE: %d\tLINE: %d\n", t1 -> typeKind, t2 -> typeKind, a -> line);
				exit(0);
			}

			ret = passValue(t1, t2);

			if(ret == 0) {

				fprintf(stderr, "\nAST TYPING ERROR(STAT_ASSIGN): UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}
		}
		else if( a -> nodeType == STAT_RETURN ) {
			
			type_exp(a -> nodeStruct.stat -> u.retCommand.exp00Node, NULL);

			if(a -> nodeStruct.stat -> u.retCommand.exp00Node != NULL) {
				t1 = a -> nodeStruct.stat -> u.retCommand.exp00Node -> nodeStruct.exp -> typing;

				if(retList -> typing == NULL) {
					retList -> typing = t1;
				}
				else {
					tempList = new(typeList);
					tempList -> typing = t1;
					tempList -> nextTypingNode = retList;
					retList = tempList;
				}
			}
		}
		else if( a -> nodeType == STAT_FUNC_CALL ) {
		
			type_call(a -> nodeStruct.stat -> u.callFunc);	
		}
		else {

			fprintf(stderr, "\nAST TYPING ERROR(STAT): UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}

		//type_tree(a->right);
		type_stat(a->left, retList);
	}
}


static void
type_call ( Call *a ) {//		TODO INACABADO

	int ret;
	typeList *funcParameters = NULL; // DADO PELA ESTRUTURA DA AMARRAÇÃO?
	typeList *arguments = NULL;

	if( a != NULL ) {

		arguments = new(typeList);
		arguments -> typing = NULL;
		arguments -> nextTypingNode = NULL;

		funcParameters = new(typeList);
		funcParameters -> typing = NULL;
		funcParameters -> nextTypingNode = NULL;

		type_exp(a -> expressionNode, arguments);

		ret = verifyFuncParameters(funcParameters, arguments);

		if(ret != 1)
			printFuncCallErrorMsg(ret, a);
	}
}


static void
type_block( AST_Node *a, typeList *retList ) {

	if( a != NULL ) {
		
		if ( a -> nodeType == DEF_VAR ) {
			
			type_def(a);
			type_stat(a->right, retList);
		}
		else if( a -> node == STAT ) {
			type_stat(a, retList);
		}
		else {

			fprintf(stderr, "\nAST TYPING ERROR(BLOCK): UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}
	}	
}


static void
type_def ( AST_Node *a ) {

	Typing *funcReturn = NULL;
	typeList *returns = NULL;

	if( a != NULL ) {
		
		if( a -> nodeType == DEF_VAR ) {
			
			//type_type(a -> nodeStruct.def -> u.defVar -> dataTypeNode);
			type_var(a -> nodeStruct.def -> u.defVar -> varListNode);

			if(a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> typing == NULL)
				a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> typing = getTypingFromType(a -> nodeStruct.def -> u.defVar -> dataTypeNode);
		}
		else if( a -> nodeType == DEF_FUNC )  {

			if(a -> nodeStruct.def -> u.func.tagReturnType == 1)
				funcReturn = getTypingFromType(a -> nodeStruct.def -> u.func.ret.dataTypeNode);
			else
				funcReturn = createTypingValue(VOID, NONE, 0, 0.0, NULL);

			returns = new(typeList);
			returns -> typing = NULL;
			returns -> nextTypingNode = NULL;

			type_params(a -> nodeStruct.def -> u.func.param); 			// TODO DAQUI PRECISO OS PARAMETROS PRA COMPARAR COM FUNC CALL
			type_block(a -> nodeStruct.def -> u.func.block, returns);

			if(verifyTypeList(funcReturn, returns) != 1) {

				printf("\nDEF_FUNC RETURN TYPE NOT MATCHING\tFUNC NAME: %s\tLINE: %d\n", a -> nodeStruct.def -> u.func.funcName, a -> line);
				exit(0);
			}
		}
		else {

			fprintf(stderr, "\nAST TYPING ERROR(DEF): UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}

		type_tree(a->left);
		//type_tree(a->right);
	}
}


void
type_tree( AST_Node *a ) {
	 
 	if( a != NULL )
 	   switch( a -> node ) {
			case DEF:  type_def(a); break;
			case VAR:  type_var(a); break;
			case EXPR: type_exp(a, NULL); break;
			case STAT: type_stat(a, NULL); break;
			//case TYPE: type_type(a); break;
	   	 	default: fprintf(stderr, "\nAST TYPING ERROR: UNKNOWN NODE\n"); exit(0);
	 	}
}