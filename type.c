#include <string.h>
#include "ast.h"

typedef struct typeList typeList;

static void* myMalloc ( size_t size );
static void type_def(AST_Node *a);
static void type_block(AST_Node *a);
static void type_call (Call *a);
static void type_stat (AST_Node *a, typeList *retList );
//static void type_type(AST_Node *a);
static void type_var(AST_Node *a);

static Typing* getTypingFromType ( AST_Node *a );
static int verifySameType ( Typing *type1, Typing *type2 );
static int verifyTypeList ( Typing *type, typeList *types );
static int verifyFuncParameters ( typeList *parameters, typeList *arguments );
static Typing * convertTo ( Typing *type, typeEnum thisType );
static int verifyBoolean ( Typing *type );
static Typing * createTypingValue ( typeEnum type, valueEnum valueType, int intValue, float floatValue, const char *string );
static int verifyAttributionType ( Typing *type1, Typing *type2, Typing *t1, Typing *t2 );
static int passValue ( Typing *type1, Typing *type2 );
static Typing * logicExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind );
static Typing * binaryExpressions( Typing *type1, Typing *type2, nodeTypeEnum kind );
static void checkArgumentsTypes ( AST_Node *a, typeList *arguments );

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
		fprintf(stderr, "MEMORY SHORTAGE ERROR\n"); 
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

		return typing;
	}
}


// Returns 1 if Typings are equal, 0 if not
static int
verifySameType ( Typing *type1, Typing *type2 ) {

	int ret = 0;

	if(type1 -> typeKind == type2 -> typeKind) {
		if(type1 -> typeKind == ARRAY)
			ret = verifySameType(type1 -> nextTyping, type2 -> nextTyping);
		else
			return ret = 1;
	}
	else 
		return ret;
}


// Returns 1 if all types in the list match with typing of comparison, 0 if not
static int
verifyTypeList ( Typing *type, typeList *types ) {

	typeList *temp = NULL;
	int ret = 1;

	if(types == NULL && type -> typeKind == VOID)
		return 1;
	else if(types != NULL && type -> typeKind != VOID)
		while(types != NULL && ret == 1) {

			ret = verifySameType(type, types -> typing);

			temp = types -> nextTypingNode;
			free(types);
			types = temp;
		}

	if(types != NULL) {
		ret = 0;
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

	if(parameters == NULL && arguments == NULL)
		return 1;
	else if(parameters != NULL && arguments != NULL)
		while(parameters != NULL && arguments != NULL && ret == 1) {
			ret = verifySameType(parameters -> typing, arguments -> typing);

			temp = parameters -> nextTypingNode;
			free(parameters);
			parameters = temp;
			temp = arguments -> nextTypingNode;
			free(arguments);
			arguments = temp;
		}

	if(parameters != NULL) {
		if(ret == 1)
			ret = 2;
		while(parameters != NULL) {
			temp = parameters -> nextTypingNode;
			free(parameters);
			parameters = temp;
		}
	}
	
	if(arguments != NULL) {
		if(ret == 1)
			ret = 3;
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

	if(thisType == INTEGER) {
		convertedTyping -> typeKind = INTEGER;
		convertedTyping -> type = INT_VALUE;
	}
	else {
		convertedTyping -> typeKind = FLOAT;
		convertedTyping -> type = FLOAT_VALUE;
	}

	while(temp -> typeKind == ARRAY)
		temp = temp -> nextTyping;

	if(thisType == INTEGER) {
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

	if(type -> typeValue.intValue == 0)
		return 0;
	else
		return 1;
}


// Creates a new Typing structure with it's respective value inside
static Typing *
createTypingValue ( typeEnum typeNum, valueEnum valueType, int intValue, float floatValue, const char *string ) {

	Typing *type = new(Typing);

	type -> typeKind = typeNum;
	type -> type = valueType;

	if(valueType == INT_VALUE)
		type -> typeValue.intValue = intValue;
	else if(valueType == FLOAT_VALUE)
		type -> typeValue.floatValue = floatValue;
	else if(typeNum == STRING_TYPE && valueType == STRING)
		type -> typeValue.string = string;

	return type;
}


// Returns 1 if Typings have the same type (ignores array type), 0 if not
static int
verifyAttributionType ( Typing *type1, Typing *type2, Typing *t1, Typing *t2 ) {

	t1 = type1;
	t2 = type2;

	if(t1 -> typeKind == ARRAY)
		while(t1 -> typeKind == ARRAY)
			t1 = t1 -> nextTyping;

	if(t2 -> typeKind == ARRAY)
		while(t2 -> typeKind == ARRAY)
			t2 = t2 -> nextTyping;

	return verifySameType(t1, t2);
}


// Pass value from type2 to type1, returns 1 if done with no problems, 0 if value inside Typings is not matching
static int
passValue ( Typing *type1, Typing *type2 ) {

	if(type2 -> type == NONE || (type2 -> type == INT_VALUE && type2 -> typeKind != INTEGER) || (type2 -> type == STRING && type2 -> typeKind != STRING_TYPE)
		|| (type2 -> type != STRING && type2 -> typeKind == STRING_TYPE) || (type1 -> type == STRING && type1 -> typeKind != STRING_TYPE)
		|| (type2 -> type == FLOAT_VALUE && type2 -> typeKind != FLOAT) || (type1 -> type != NONE && (type1 -> type != type2 -> type)
		|| (type1 -> type == INT_VALUE && type1 -> typeKind != INTEGER) || (type1 -> type == FLOAT_VALUE && type1 -> typeKind != FLOAT)
		|| (type1 -> type != STRING && type1 -> typeKind == STRING_TYPE)))
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

	if(type2 == NULL)
		logic -> typeValue.intValue = !(type1 -> typeValue.intValue);
	else{
		if(type1 -> typeKind != type2 -> typeKind) {
			if(type1 -> typeKind != INT_VALUE)
				t1 = convertTo(type1, INTEGER);
			if(type2 -> typeKind != INT_VALUE)
				t2 = convertTo(type2, INTEGER);

			if(t1 == NULL && t2 != NULL) {
				if(kind == EXPR_OR)
					logic -> typeValue.intValue = (type1 -> typeValue.intValue || t2 -> typeValue.intValue);
				else if(kind == EXPR_AND)
					logic -> typeValue.intValue = (type1 -> typeValue.intValue && t2 -> typeValue.intValue);
			}
			else if(t1 != NULL && t2 == NULL) {
				if(kind == EXPR_OR)
					logic -> typeValue.intValue = (t1 -> typeValue.intValue || type2 -> typeValue.intValue);
				else if(kind == EXPR_AND)
					logic -> typeValue.intValue = (t1 -> typeValue.intValue && type2 -> typeValue.intValue);
			}
			else {
				if(kind == EXPR_OR)
					logic -> typeValue.intValue = (t1 -> typeValue.intValue || t2 -> typeValue.intValue);
				else if(kind == EXPR_AND)
					logic -> typeValue.intValue = (t1 -> typeValue.intValue && t2 -> typeValue.intValue);
			}

			free(t1);
			free(t2);
		}
	}
	
	return logic;
}


// Creates a new Typing holding the result of a binary operation or comparison between two Typings from expressions, converts their values if necessary
static Typing *
binaryExpressions( Typing *type1, Typing *type2, nodeTypeEnum kind ) {

	Typing *binary = new(Typing), *t1 = NULL, *t2 = NULL;

	if(type1 -> typeKind != type2 -> typeKind) {
		if(type1 -> typeKind != FLOAT_VALUE)
			t1 = convertTo(type1, FLOAT);
		if(type2 -> typeKind != FLOAT_VALUE)
			t2 = convertTo(type2, FLOAT);

		if(t1 == NULL && t2 != NULL) {
			if(kind == EXPR_EQUAL)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue == t2 -> typeValue.intValue);
			else if(kind == EXPR_LEEQ)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue <= t2 -> typeValue.intValue);
			else if(kind == EXPR_GREQ)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue >= t2 -> typeValue.intValue);
			else if(kind == EXPR_GREATER)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue > t2 -> typeValue.intValue);
			else if(kind == EXPR_LESS)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue < t2 -> typeValue.intValue);
			else if(kind == EXPR_NOEQ)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue != t2 -> typeValue.intValue);
			else if(kind == EXPR_ADD)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue + t2 -> typeValue.intValue);
			else if(kind == EXPR_MIN)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue - t2 -> typeValue.intValue);
			else if(kind == EXPR_MUL)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue * t2 -> typeValue.intValue);
			else if(kind == EXPR_DIV)
				binary -> typeValue.intValue = (type1 -> typeValue.intValue / t2 -> typeValue.intValue);
		}
		else if(t1 != NULL && t2 == NULL) {
			if(kind == EXPR_EQUAL)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue == type2 -> typeValue.intValue);
			else if(kind == EXPR_LEEQ)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue <= type2 -> typeValue.intValue);
			else if(kind == EXPR_GREQ)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue >= type2 -> typeValue.intValue);
			else if(kind == EXPR_GREATER)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue > type2 -> typeValue.intValue);
			else if(kind == EXPR_LESS)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue < type2 -> typeValue.intValue);
			else if(kind == EXPR_NOEQ)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue != type2 -> typeValue.intValue);
			else if(kind == EXPR_ADD)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue + type2 -> typeValue.intValue);
			else if(kind == EXPR_MIN)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue - type2 -> typeValue.intValue);
			else if(kind == EXPR_MUL)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue * type2 -> typeValue.intValue);
			else if(kind == EXPR_DIV)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue / type2 -> typeValue.intValue);
		}
		else {
			if(kind == EXPR_EQUAL)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue == t2 -> typeValue.intValue);
			else if(kind == EXPR_LEEQ)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue <= t2 -> typeValue.intValue);
			else if(kind == EXPR_GREQ)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue >= t2 -> typeValue.intValue);
			else if(kind == EXPR_GREATER)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue > t2 -> typeValue.intValue);
			else if(kind == EXPR_LESS)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue < t2 -> typeValue.intValue);
			else if(kind == EXPR_NOEQ)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue != t2 -> typeValue.intValue);
			else if(kind == EXPR_ADD)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue + t2 -> typeValue.intValue);
			else if(kind == EXPR_MIN)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue - t2 -> typeValue.intValue);
			else if(kind == EXPR_MUL)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue * t2 -> typeValue.intValue);
			else if(kind == EXPR_DIV)
				binary -> typeValue.intValue = (t1 -> typeValue.intValue / t2 -> typeValue.intValue);
		}

		free(t1);
		free(t2);
	}
	
	return binary;
}


// TODO INACABADO
// Checks the expression list, populating the list received for each exp visited
static void
checkArgumentsTypes ( AST_Node *a, typeList *arguments ) {

	int intVal;
	float floatVal;
	Typing *typing = NULL, *temp = NULL;
	typeList *tempList = NULL, *tempListSon = NULL;

	if( a != NULL ) {

		if ( a -> nodeType == EXPR_VAR ) {
			
			type_var(a -> nodeStruct.exp -> u.varNode);

			if(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type == INT_VALUE)
				intVal = a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> typeValue.intValue;
			else if(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type == FLOAT_VALUE)
				floatVal = a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> typeValue.floatValue;
			else if(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type == NONE) {
				intVal = 0;
				floatVal = 0.0;
			}

			typing = createTypingValue(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> typeKind, a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type, intVal, floatVal, NULL);
		}
		else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			type_call(a -> nodeStruct.exp -> u.functionCall);

			//typing = createTypingValue( /* TIPO DADO PELA TABELA? */, NONE, 0, 0.0, NULL);
		}
		else if ( a -> nodeType == EXPR_NEW ) {
			
			//type_type(a->left);
			checkArgumentsTypes(a->right, arguments);
			
			if(a -> right -> nodeStruct.exp -> typing -> typeKind != INTEGER) {

				printf("EXPR_NEW EXP FOR SIZE IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", a -> right -> nodeStruct.exp -> typing -> typeKind, a -> line);
				exit(0);
			}

			typing = getTypingFromType(a -> left);
		}
		else if ( a -> nodeType == EXPR_INT || a -> nodeType == EXPR_HEXA || a -> nodeType == EXPR_CHAR ) {

			typing = createTypingValue(INTEGER, INT_VALUE, a -> nodeStruct.exp -> u.ki, 0.0, NULL);
		}
		else if ( a -> nodeType == EXPR_FLOAT ) {

			typing = createTypingValue(FLOAT, FLOAT_VALUE, 0, a -> nodeStruct.exp -> u.kf, NULL);
		}
		else if ( a -> nodeType == EXPR_LIT ) {

			typing = createTypingValue(STRING_TYPE, STRING, 0, 0.0, a -> nodeStruct.exp -> u.lit);
		}
		else if ( a -> nodeType == EXPR_OR ) {
			
			typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_OR);
		}
		else if ( a -> nodeType == EXPR_AND ) {
			
			typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_AND);
		}
		else if ( a -> nodeType == EXPR_EQUAL ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_EQUAL);
		}
		else if ( a -> nodeType == EXPR_LEEQ ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_LEEQ);
		}
		else if ( a -> nodeType == EXPR_GREQ ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_GREQ);
		}
		else if ( a -> nodeType == EXPR_GREATER ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_GREATER);
		}
		else if ( a -> nodeType == EXPR_LESS ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_LESS);
		}
		else if ( a -> nodeType == EXPR_NOEQ ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_NOEQ);
		}
		else if ( a -> nodeType == EXPR_ADD ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_ADD);
		}
		else if ( a -> nodeType == EXPR_MIN ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_MIN);
		}
		else if ( a -> nodeType == EXPR_MUL ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_MUL);
		}
		else if ( a -> nodeType == EXPR_DIV ) {
			
			if((a -> right -> nodeStruct.exp -> typing -> type == INT_VALUE && a -> right -> nodeStruct.exp -> typing -> typeValue.intValue == 0)
				|| (a -> right -> nodeStruct.exp -> typing -> type == FLOAT_VALUE && a -> right -> nodeStruct.exp -> typing -> typeValue.floatValue == 0.0)) {

				printf("EXPR_DIV EXP IN THE DIVIDER IS ZERO\tLINE: %d\n", a -> line);
				exit(0);
			}

			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_DIV);
		}
		else if ( a -> nodeType == EXPR_NOT ) {
			
			typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, NULL, EXPR_OR);
		}
		else if ( a -> nodeType == EXPR_NEG ) {
			
			typing = new(Typing);

			if( a -> left -> nodeStruct.exp -> typing -> type == INT_VALUE ) {
				typing -> typeKind = INTEGER;
				typing -> type = INT_VALUE;
				typing -> typeValue.intValue = a -> left -> nodeStruct.exp -> typing -> typeValue.intValue;
			}
			else if( a -> left -> nodeStruct.exp -> typing -> type == FLOAT_VALUE ) {
				typing -> typeKind = FLOAT;
				typing -> type = FLOAT_VALUE;
				typing -> typeValue.floatValue = a -> left -> nodeStruct.exp -> typing -> typeValue.floatValue;
			}
		}
		else {

			fprintf(stderr, "AST TYPING ERROR: UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}
		
		a -> nodeStruct.exp -> typing = typing;

		if(a -> nodeStruct.exp -> nextExpNode != NULL) {
			tempList = new(typeList);
			tempList -> typing = typing;
			tempList -> nextTypingNode = NULL;

			if(arguments != NULL)
				tempList -> nextTypingNode = arguments;
			else {

				tempListSon = new(typeList);
				tempListSon -> typing = a -> nodeStruct.exp -> nextExpNode -> nodeStruct.exp -> typing;
				tempListSon -> nextTypingNode = NULL;

				tempList -> nextTypingNode = tempListSon;
			}
			arguments = tempList;
		}

		checkArgumentsTypes(a->left, arguments);
		checkArgumentsTypes(a->right, arguments);

		checkArgumentsTypes(a -> nodeStruct.exp -> nextExpNode, arguments);
	}

}


/*static void
type_type ( AST_Node *a ) {
	
	if( a != NULL ) {

		printf("TYPE: ");
		printf("%s", a -> nodeStruct.type -> baseType);
	}	
}*/


static void//			TODO INCACABADO
type_exp ( AST_Node *a ) {

	int intVal;
	float floatVal;
	Typing *typing = NULL, *temp = NULL;

	if( a != NULL ) {

		if ( a -> nodeType == EXPR_VAR ) {
			
			type_var(a -> nodeStruct.exp -> u.varNode);

			if(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type == INT_VALUE)
				intVal = a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> typeValue.intValue;
			else if(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type == FLOAT_VALUE)
				floatVal = a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> typeValue.floatValue;
			else if(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type == NONE) {
				intVal = 0;
				floatVal = 0.0;
			}

			typing = createTypingValue(a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> typeKind, a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing -> type, intVal, floatVal, NULL);
		}
		else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			type_call(a -> nodeStruct.exp -> u.functionCall);

			//typing = createTypingValue( /* TIPO DADO PELA TABELA? */, NONE, 0, 0.0, NULL);
		}
		else if ( a -> nodeType == EXPR_NEW ) {
			
			//type_type(a->left);
			type_exp(a->right);
			
			if(a -> right -> nodeStruct.exp -> typing -> typeKind != INTEGER) {

				printf("EXPR_NEW EXP FOR SIZE IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", a -> right -> nodeStruct.exp -> typing -> typeKind, a -> line);
				exit(0);
			}

			typing = getTypingFromType(a -> left);
		}
		else if ( a -> nodeType == EXPR_INT || a -> nodeType == EXPR_HEXA || a -> nodeType == EXPR_CHAR ) {

			typing = createTypingValue(INTEGER, INT_VALUE, a -> nodeStruct.exp -> u.ki, 0.0, NULL);
		}
		else if ( a -> nodeType == EXPR_FLOAT ) {

			typing = createTypingValue(FLOAT, FLOAT_VALUE, 0, a -> nodeStruct.exp -> u.kf, NULL);
		}
		else if ( a -> nodeType == EXPR_LIT ) {

			typing = createTypingValue(STRING_TYPE, STRING, 0, 0.0, a -> nodeStruct.exp -> u.lit);
		}
		else if ( a -> nodeType == EXPR_OR ) {
			
			typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_OR);
		}
		else if ( a -> nodeType == EXPR_AND ) {
			
			typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_AND);
		}
		else if ( a -> nodeType == EXPR_EQUAL ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_EQUAL);
		}
		else if ( a -> nodeType == EXPR_LEEQ ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_LEEQ);
		}
		else if ( a -> nodeType == EXPR_GREQ ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_GREQ);
		}
		else if ( a -> nodeType == EXPR_GREATER ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_GREATER);
		}
		else if ( a -> nodeType == EXPR_LESS ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_LESS);
		}
		else if ( a -> nodeType == EXPR_NOEQ ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_NOEQ);
		}
		else if ( a -> nodeType == EXPR_ADD ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_ADD);
		}
		else if ( a -> nodeType == EXPR_MIN ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_MIN);
		}
		else if ( a -> nodeType == EXPR_MUL ) {
			
			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_MUL);
		}
		else if ( a -> nodeType == EXPR_DIV ) {
			
			if((a -> right -> nodeStruct.exp -> typing -> type == INT_VALUE && a -> right -> nodeStruct.exp -> typing -> typeValue.intValue == 0)
				|| (a -> right -> nodeStruct.exp -> typing -> type == FLOAT_VALUE && a -> right -> nodeStruct.exp -> typing -> typeValue.floatValue == 0.0)) {

				printf("EXPR_DIV EXP IN THE DIVIDER IS ZERO\tLINE: %d\n", a -> line);
				exit(0);
			}

			typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, EXPR_DIV);
		}
		else if ( a -> nodeType == EXPR_NOT ) {
			
			typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, NULL, EXPR_OR);
		}
		else if ( a -> nodeType == EXPR_NEG ) {
			
			typing = new(Typing);

			if( a -> left -> nodeStruct.exp -> typing -> type == INT_VALUE ) {
				typing -> typeKind = INTEGER;
				typing -> type = INT_VALUE;
				typing -> typeValue.intValue = a -> left -> nodeStruct.exp -> typing -> typeValue.intValue;
			}
			else if( a -> left -> nodeStruct.exp -> typing -> type == FLOAT_VALUE ) {
				typing -> typeKind = FLOAT;
				typing -> type = FLOAT_VALUE;
				typing -> typeValue.floatValue = a -> left -> nodeStruct.exp -> typing -> typeValue.floatValue;
			}
		}
		else {

			fprintf(stderr, "AST TYPING ERROR: UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}
		
		a -> nodeStruct.exp -> typing = typing;

		type_exp(a->left);
		type_exp(a->right);

		//type_exp(a -> nodeStruct.exp -> nextExpNode);
	}
}


static void//		TODO INACABADO
type_var( AST_Node *a ) {
	
	Typing *type = NULL;

	if( a != NULL ) {

		if ( a -> nodeType == VAR_UNIQUE ) {

			//if(a -> nodeStruct.var -> typing == NULL)
			//	type = createTypingValue( /* TIPO DADO PELA TABELA? */, /* TIPO VALOR DADO PELA TABELA? */, /* VALOR DADO PELA TABELA? */, /* VALOR DADO PELA TABELA? */, /* STRING DADA PELA TABELA? */ );
		}
		else if ( a -> nodeType == VAR_INDEXED ) {
		
			type_exp(a->left);
			type_exp(a->right);

			if(a -> right -> nodeStruct.exp -> typing -> typeKind != INTEGER) {

				printf("VAR_INDEXED EXP IN THE RIGHT IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", a -> right -> nodeStruct.exp -> typing -> typeKind, a -> line);
				exit(0);
			}

			//if(a -> nodeStruct.var -> typing == NULL)
			//	type = createTypingValue( /* TIPO DADO PELA TABELA? */, /* TIPO VALOR DADO PELA TABELA? */, /* VALOR DADO PELA TABELA? */, /* VALOR DADO PELA TABELA? */, /* STRING DADA PELA TABELA? */  );			
		}
		else {

			fprintf(stderr, "AST TYPING ERROR: UNEXPECTED NODE AT LINE %d\n", a -> line);
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

	if( p != NULL ) {

		//type_type(p -> var -> dataTypeNode);
		type_var(p -> var -> varListNode);

		if(p -> var -> varListNode -> nodeStruct.var -> typing == NULL)//		TODO PRECISA?
			p -> var -> varListNode -> nodeStruct.var -> typing = getTypingFromType(p -> var -> dataTypeNode);

		type_params(p -> proxParam);
	}
}


static void
type_stat ( AST_Node *a, typeList *retList ) {
	
	int ret;
	Typing *type = NULL, *t1 = NULL, *t2 = NULL, *temp = NULL;
	typeList *tempList = NULL;

	if( a != NULL ) {
		
		if( a -> nodeType == STAT_WHILE ) {
			
			type_exp(a -> nodeStruct.stat -> u.whileLoop.exp00Node);
			type_stat(a -> nodeStruct.stat -> u.whileLoop.commandListNode, retList);

			if(a -> nodeStruct.stat -> u.whileLoop.exp00Node -> nodeStruct.exp -> typing -> typeKind != INTEGER)
				temp = convertTo(a -> nodeStruct.stat -> u.whileLoop.exp00Node -> nodeStruct.exp -> typing, INTEGER);
			
			ret = verifyBoolean(temp);

			if(ret == -1) {

				fprintf(stderr, "AST TYPING ERROR: UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}

			type = createTypingValue(INTEGER, INT_VALUE, ret, 0.0, NULL);
			a -> nodeStruct.stat -> typing = type;
		}
		else if ( a -> nodeType == STAT_IF ) {

			type_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.block, retList);

			if(a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing -> typeKind != INTEGER)
				temp = convertTo(a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing, INTEGER);

			ret = verifyBoolean(temp);

			if(ret == -1) {
					
				fprintf(stderr, "AST TYPING ERROR: UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}

			type = createTypingValue(INTEGER, INT_VALUE, ret, 0.0, NULL);
			a -> nodeStruct.stat -> typing = type;
		}
		else if( a -> nodeType == STAT_IFELSE ) {
			
			type_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.block, retList);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo, retList);

			if(a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing -> typeKind != INTEGER)
				temp = convertTo(a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing, INTEGER);

			ret = verifyBoolean(temp);

			if(ret == -1) {
					
				fprintf(stderr, "AST TYPING ERROR: UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}

			type = createTypingValue(INTEGER, INT_VALUE, ret, 0.0, NULL);
			a -> nodeStruct.stat -> typing = type;
		}
		else if( a -> nodeType == STAT_ASSIGN ) {

			type_var(a -> nodeStruct.stat -> u.assign.varNode);
			type_exp(a -> nodeStruct.stat -> u.assign.exp00Node);

			if(verifyAttributionType(a -> nodeStruct.stat -> u.assign.varNode -> nodeStruct.var -> typing, a -> nodeStruct.stat -> u.assign.exp00Node -> nodeStruct.exp -> typing, t1, t2) == 0) {

				printf("STAT_ASSIGN VAR AND EXP ARE NOT THE SAME TYPE\tVAR ENUM TYPE: %d\tEXP ENUM TYPE: %d\tLINE: %d\n", a -> nodeStruct.stat -> u.assign.varNode -> nodeStruct.var -> typing -> typeKind, a -> nodeStruct.stat -> u.assign.exp00Node -> nodeStruct.exp -> typing -> typeKind, a -> line);
				exit(0);
			}

			ret = passValue(t1, t2);

			if(ret == 0) {

				fprintf(stderr, "AST TYPING ERROR: UNEXPECTED TYPING VALUE AT LINE %d\n", a -> line);
				exit(0);
			}
		}
		else if( a -> nodeType == STAT_RETURN ) {
			
			type_exp(a -> nodeStruct.stat -> u.retCommand.exp00Node);

			tempList = new(typeList);
			tempList -> typing = a -> nodeStruct.stat -> u.retCommand.exp00Node -> nodeStruct.exp -> typing;

			if(retList != NULL)
				tempList -> nextTypingNode = retList;
			retList = tempList;
		}
		else if( a -> nodeType == STAT_FUNC_CALL ) {
		
			type_call(a -> nodeStruct.stat -> u.callFunc);	
		}
		else {

			fprintf(stderr, "AST TYPING ERROR: UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}

		//type_tree(a->right);
		type_stat(a->left, retList);
	}	
}


static void
type_call ( Call *a ) {//		TODO INACABADO

	int ret;
	typeList *arguments = NULL;
	typeList *funcParameters; //= /* DADO PELA ESTRUTURA DA AMARRAÇÃO? */;

	if( a != NULL ) {

		checkArgumentsTypes(a -> expressionNode, arguments);

		ret = verifyFuncParameters(funcParameters, arguments);

		if(ret != 1) {
			if(ret == 0)
				printf("FUNC_CALL TYPES NOT MATCHING\tFUNC NAME: %s\n", a -> funcName);
			else if(ret == 2)
				printf("FUNC_CALL MISSING ARGUMENTS\tFUNC NAME: %s\n", a -> funcName);
			else if(ret == 3)
				printf("FUNC_CALL TOO MANY ARGUMENTS\tFUNC NAME: %s\n", a -> funcName);

			exit(0);
		}
	}

	free(a);
}


static void
block_type( AST_Node *a, typeList *retList ) {

	if( a != NULL ) {
		
		if ( a -> nodeType == DEF_VAR ) {
			
			type_def(a);
			type_stat(a->right, retList);
		}
		else if( a -> node == STAT ) {
			type_stat(a, retList);
		}
		else {

			fprintf(stderr, "AST TYPING ERROR: UNEXPECTED NODE AT LINE %d\n", a -> line);
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
		}
		else if( a -> nodeType == DEF_FUNC )  {

			if(a -> nodeStruct.def -> u.func.tagReturnType == 1)
				funcReturn = getTypingFromType(a -> nodeStruct.def -> u.func.ret.dataTypeNode);
			else
				funcReturn = createTypingValue(VOID, NONE, 0, 0.0, NULL);

			type_params(a -> nodeStruct.def -> u.func.param); 			// TODO DAQUI PRECISO OS PARAMETROS PRA COMPARAR COM FUNC CALL
			block_type(a -> nodeStruct.def -> u.func.block, returns);

			if(verifyTypeList(funcReturn, returns) != 1) {

				printf("DEF_FUNC RETURN TYPE NOT MATCHING\tFUNC NAME: %s\tLINE: %d\n", a -> nodeStruct.def -> u.func.funcName, a -> line);
				exit(0);
			}
		}
		else {

			fprintf(stderr, "AST TYPING ERROR: UNEXPECTED NODE AT LINE %d\n", a -> line);
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
			case EXPR: type_exp(a); break;
			//case STAT: type_stat(a); break;	   
			//case TYPE: type_type(a); break; // TODO TALVEZ NÃO PRECISE
	   	 	default: fprintf(stderr, "AST TYPING ERROR: UNKNOWN NODE\n"); exit(0);
	 	}
}