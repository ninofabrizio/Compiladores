#include <string.h>
#include "ast.h"

typedef struct typeList typeList;

static void* myMalloc ( size_t size );
static void type_def ( AST_Node *a );
static void type_block ( AST_Node *a, typeList *retList );
static void type_call ( Call *a );
static void type_stat ( AST_Node *a, typeList *retList );
static void type_var ( AST_Node *a );
static void type_exp ( AST_Node *a, typeList *arguments );

static Typing* getTypingFromType ( AST_Node *a );
static int verifySameType ( Typing *type1, Typing *type2 );
static int verifyTypeList ( Typing *type, typeList *types );
static int verifyFuncParameters ( typeList *parameters, typeList *arguments );
static Typing * convertTo ( Typing *type, typeEnum thisType );
static Typing * createTypingValue ( typeEnum type );
static Typing * logicExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind );
static Typing * binaryExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind );
static int notOfType ( typeEnum type, typeEnum comparison );
static void verifyIfLiterals ( Typing *type1, Typing *type2, const char* enumT, int line );
static void getParametersList ( typeList * funcParameters, AST_Node *funcDefNode );

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

	if(a != NULL) {

		typing = new(Typing);

		if(a -> nodeType == TYPE_INT || a -> nodeType == TYPE_CHAR)
			typing -> typeKind = INTEGER;
		else if(a -> nodeType == TYPE_FLOAT)
			typing -> typeKind = FLOAT;

		for(i = 0; i < a -> nodeStruct.type -> arraySequence ; i++) {
			temp = new(Typing);
			temp -> typeKind = ARRAY;

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

	if(ret == 1 && ((arguments == NULL && parameters != NULL)
					|| (arguments != NULL && arguments -> typing == NULL && parameters != NULL && parameters -> typing != NULL)))
		ret = 2;
	else if(ret == 1 && ((parameters == NULL && arguments != NULL)
						|| (parameters != NULL && parameters -> typing == NULL && arguments != NULL && arguments -> typing != NULL)))
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

	if(thisType == INTEGER)
		convertedTyping -> typeKind = INTEGER;
	else
		convertedTyping -> typeKind = FLOAT;

	return convertedTyping;
}


// Creates a new Typing structure with it's respective value inside
static Typing *
createTypingValue ( typeEnum typeNum/*, valueEnum valueType, int intValue, float floatValue, const char *string*/ ) {

	Typing *type = new(Typing);

	type -> typeKind = typeNum;
	type -> nextTyping = NULL;

	return type;
}


// Creates a new Typing holding the result of a logic comparison between one or two Typings from expressions, converts their values if necessary
static Typing *
logicExpressions ( Typing *type1, Typing *type2, nodeTypeEnum kind ) {

	Typing *logic = new(Typing), *t1 = NULL, *t2 = NULL;
	logic -> typeKind = INTEGER;

	if(type1 -> typeKind != INTEGER)
		t1 = convertTo(type1, INTEGER);
	else
		t1 = type1;
		
	if(type2 -> typeKind != INTEGER)
		t2 = convertTo(type2, INTEGER);
	else
		t2 = type2;

	if(type1 -> typeKind != INTEGER)
		free(t1);
	if(type2 -> typeKind != INTEGER)
		free(t2);
	
	return logic;
}


// Creates a new Typing holding the result of a binary operation or comparison between two Typings from expressions, converts their values if necessary
static Typing *
binaryExpressions( Typing *type1, Typing *type2, nodeTypeEnum kind ) {

	Typing *binary = new(Typing), *t1 = NULL, *t2 = NULL;

	if(type1 -> typeKind != type2 -> typeKind && type1 -> typeKind != FLOAT)
		t1 = convertTo(type1, FLOAT);
	else
		t1 = type1;
	
	if(type1 -> typeKind != type2 -> typeKind && type2 -> typeKind != FLOAT)
		t2 = convertTo(type2, FLOAT);
	else
		t2 = type2;

	if(t1 -> typeKind == INTEGER)
		binary -> typeKind = INTEGER;
	else 
		binary -> typeKind = FLOAT;

	if(type1 -> typeKind != FLOAT)
		free(t1);
	if(type2 -> typeKind != FLOAT)
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
			printf("\nFUNC_CALL ARGUMENT TYPES NOT MATCHING\tFUNC NAME: %s\n", a -> funcName);
		else
			printf("\nFUNC_CALL ARGUMENT TYPES NOT MATCHING\tFUNC NAME: %s\tLINE: %d\n", a -> funcName, a -> expressionNode -> line);
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


// Populates the list of typyings from the received function's parameters
static void
getParametersList ( typeList * funcParameters, AST_Node *funcDefNode ) {

	Param *param = funcDefNode -> nodeStruct.def -> u.func.param;
	typeList *temp1 = NULL, *temp2 = funcParameters;
	Var *var = NULL;

	while(param != NULL) {

		var = param -> var -> varListNode -> nodeStruct.var;

		if(funcParameters -> typing == NULL)
			funcParameters -> typing = var -> typing;
		else {

			temp1 = new(typeList);
			temp1 -> typing = var -> typing;
			temp1 -> nextTypingNode = NULL;

			temp2-> nextTypingNode = temp1;
			temp2 = temp1;
		}

		param = param -> proxParam;
	}
}


static void
type_exp ( AST_Node *a, typeList *arguments ) {

	Typing *temp = NULL;
	typeList *tempList = NULL;

	if( a != NULL ) {

		if ( a -> nodeType == EXPR_VAR ) {
			
			type_var(a -> nodeStruct.exp -> u.varNode);

			temp = a -> nodeStruct.exp -> u.varNode -> nodeStruct.var -> typing;

			while(temp -> typeKind == ARRAY)
				temp = temp -> nextTyping;

			a -> nodeStruct.exp -> typing = createTypingValue(temp -> typeKind);
		}
		else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			type_call(a -> nodeStruct.exp -> u.functionCall);

			if(a -> nodeStruct.exp -> u.functionCall -> linkedFuncNode -> nodeStruct.def -> u.func.tagReturnType == 1)
				temp = getTypingFromType(a -> nodeStruct.exp -> u.functionCall -> linkedFuncNode -> nodeStruct.def -> u.func.ret.dataTypeNode);
			else
				temp = createTypingValue(VOID);

			a -> nodeStruct.exp -> typing = temp;
		}
		else if ( a -> nodeType == EXPR_NEW ) {

			type_exp(a->right, NULL);
			
			temp = a -> right -> nodeStruct.exp -> typing;

			if(notOfType(temp -> typeKind, INTEGER)) {

				printf("\nEXPR_NEW EXP IN THE RIGHT IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", temp -> typeKind, a -> line);
				exit(0);
			}

			a -> nodeStruct.exp -> typing = getTypingFromType(a -> left);
		}
		else if ( a -> nodeType == EXPR_INT || a -> nodeType == EXPR_HEXA || a -> nodeType == EXPR_CHAR ) {

			a -> nodeStruct.exp -> typing = createTypingValue(INTEGER);
		}
		else if ( a -> nodeType == EXPR_FLOAT ) {

			a -> nodeStruct.exp -> typing = createTypingValue(FLOAT);
		}
		else if ( a -> nodeType == EXPR_LIT ) {

			a -> nodeStruct.exp -> typing = createTypingValue(STRING_TYPE);
		}
		else if ( a -> nodeType == EXPR_OR || a -> nodeType == EXPR_AND || a -> nodeType == EXPR_NOT ) {

			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			a -> nodeStruct.exp -> typing = logicExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, a -> nodeType);
		}
		else if ( a -> nodeType == EXPR_EQUAL || a -> nodeType == EXPR_LEEQ || a -> nodeType == EXPR_GREQ || a -> nodeType == EXPR_GREATER || a -> nodeType == EXPR_LESS
				|| a -> nodeType == EXPR_NOEQ || a -> nodeType == EXPR_ADD || a -> nodeType == EXPR_MIN || a -> nodeType == EXPR_MUL || a -> nodeType == EXPR_DIV ) {
			
			const char *exprType;

			if(a -> nodeType == EXPR_EQUAL)
				exprType = "EXPR_EQUAL";
			else if(a -> nodeType == EXPR_LEEQ)
				exprType = "EXPR_LEEQ";
			else if(a -> nodeType == EXPR_GREQ)
				exprType = "EXPR_GREQ";
			else if(a -> nodeType == EXPR_GREATER)
				exprType = "EXPR_GREATER";
			else if(a -> nodeType == EXPR_LESS)
				exprType = "EXPR_LESS";
			else if(a -> nodeType == EXPR_NOEQ)
				exprType = "EXPR_NOEQ";
			else if(a -> nodeType == EXPR_ADD)
				exprType = "EXPR_ADD";
			else if(a -> nodeType == EXPR_MIN)
				exprType = "EXPR_MIN";
			else if(a -> nodeType == EXPR_MUL)
				exprType = "EXPR_MUL";
			else if(a -> nodeType == EXPR_DIV)
				exprType = "EXPR_DIV";

			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			verifyIfLiterals(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, exprType, a -> line);

			a -> nodeStruct.exp -> typing = binaryExpressions(a -> left -> nodeStruct.exp -> typing, a -> right -> nodeStruct.exp -> typing, a -> nodeType);
		}
		else if ( a -> nodeType == EXPR_NEG ) {

			type_exp(a->left, NULL);

			if(a -> left -> nodeStruct.exp -> typing -> typeKind != INTEGER && a -> left -> nodeStruct.exp -> typing -> typeKind != FLOAT) {

				printf("\nEXPR_NEG EXP VALUE IS NOT INTEGER OR FLOAT\tLINE: %d\n", a -> line);
				exit(0);
			}

			if( a -> left -> nodeStruct.exp -> typing -> typeKind == INTEGER ) {
				
				a -> nodeStruct.exp -> typing = createTypingValue(INTEGER);
			}
			else if( a -> left -> nodeStruct.exp -> typing -> typeKind == FLOAT ) {
				
				a -> nodeStruct.exp -> typing = createTypingValue(INTEGER);
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

		type_exp(a -> nodeStruct.exp -> nextExpNode, arguments);
	}
}


static void
type_var( AST_Node *a ) {

	if( a != NULL ) {

		if ( a -> nodeType == VAR_UNIQUE ) {

			if(a -> nodeStruct.var -> typing == NULL && a -> nodeStruct.var -> linkedVarNode != NULL) {
				if(a -> nodeStruct.var -> linkedVarNode -> node == VAR)
					a -> nodeStruct.var -> typing = a -> nodeStruct.var -> linkedVarNode -> nodeStruct.var -> typing;
				else if(a -> nodeStruct.var -> linkedVarNode -> node == DEF)
					a -> nodeStruct.var -> typing = a -> nodeStruct.var -> linkedVarNode -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> typing;
			}
		}
		else if ( a -> nodeType == VAR_INDEXED ) {
		
			type_exp(a->left, NULL);
			type_exp(a->right, NULL);

			if(notOfType(a -> right -> nodeStruct.exp -> typing -> typeKind, INTEGER)) {

				printf("\nVAR_INDEXED EXP IN THE RIGHT IS NOT INTEGER\tEXP ENUM TYPE: %d\tLINE: %d\n", a -> right -> nodeStruct.exp -> typing -> typeKind, a -> line);
				exit(0);
			}

			if(a -> nodeStruct.var -> typing == NULL && a -> nodeStruct.var -> linkedVarNode != NULL)
				a -> nodeStruct.var -> typing = a -> nodeStruct.var -> linkedVarNode -> nodeStruct.var -> typing;		
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

			type = createTypingValue(INTEGER);
			a -> nodeStruct.stat -> typing = type;
		}
		else if ( a -> nodeType == STAT_IF ) {

			type_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, NULL);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.block, retList);

			t1 = a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing;

			if(notOfType(t1 -> typeKind, INTEGER))
				t1 = convertTo(t1, INTEGER);

			type = createTypingValue(INTEGER);
			a -> nodeStruct.stat -> typing = type;
		}
		else if( a -> nodeType == STAT_IFELSE ) {
			
			type_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, NULL);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.block, retList);
			type_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo, retList);

			t1 = a -> nodeStruct.stat -> u.ifCondition.exp00Node -> nodeStruct.exp -> typing;

			if(notOfType(t1 -> typeKind, INTEGER))
				t1 = convertTo(t1, INTEGER);

			type = createTypingValue(INTEGER);
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
		}
		else if( a -> nodeType == STAT_RETURN ) {
			
			type_exp(a -> nodeStruct.stat -> u.returnExp00Node, NULL);

			if(a -> nodeStruct.stat -> u.returnExp00Node != NULL) {
				t1 = a -> nodeStruct.stat -> u.returnExp00Node -> nodeStruct.exp -> typing;

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

		type_stat(a->left, retList);
	}
}


static void
type_call ( Call *a ) {

	int ret;
	typeList *funcParameters = NULL;
	typeList *arguments = NULL;

	if( a != NULL ) {

		arguments = new(typeList);
		arguments -> typing = NULL;
		arguments -> nextTypingNode = NULL;

		funcParameters = new(typeList);
		funcParameters -> typing = NULL;
		funcParameters -> nextTypingNode = NULL;

		getParametersList(funcParameters, a -> linkedFuncNode);

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
			
			type_var(a -> nodeStruct.def -> u.defVar -> varListNode);

			if(a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> typing == NULL)
				a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> typing = getTypingFromType(a -> nodeStruct.def -> u.defVar -> dataTypeNode);
		}
		else if( a -> nodeType == DEF_FUNC )  {

			if(a -> nodeStruct.def -> u.func.tagReturnType == 1)
				funcReturn = getTypingFromType(a -> nodeStruct.def -> u.func.ret.dataTypeNode);
			else
				funcReturn = createTypingValue(VOID);

			returns = new(typeList);
			returns -> typing = NULL;
			returns -> nextTypingNode = NULL;

			type_params(a -> nodeStruct.def -> u.func.param);
			type_block(a -> nodeStruct.def -> u.func.block, returns);

			if(verifyTypeList(funcReturn, returns) != 1) {

				printf("\nDEF_FUNC RETURN TYPE NOT MATCHING\tFUNC NAME: %s\tLINE: %d\n", a -> nodeStruct.def -> u.func.funcName, a -> line);
				exit(0);
			}

			free(funcReturn);
		}
		else {

			fprintf(stderr, "\nAST TYPING ERROR(DEF): UNEXPECTED NODE AT LINE %d\n", a -> line);
			exit(0);
		}

		type_tree(a->left);
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
	   	 	default: fprintf(stderr, "\nAST TYPING ERROR: UNKNOWN NODE ENUM %d\n", a -> node); exit(0);
	 	}
}