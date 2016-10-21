#include "ast.h"

static void* myMalloc(size_t size);
static void print_def(AST_Node *a);
static void print_block(AST_Node *a);
static void print_call (Call *a);
static void print_stat (AST_Node *a);
static void print_type(AST_Node *a);
static void print_var(AST_Node *a);

#define new(T) ((T*)myMalloc(sizeof(T)))


// GENERIC NODE GENERATOR
AST_Node*
new_ast_node ( int node, int node_type, AST_Node* left, AST_Node* right, AST_Node* center, int line ) {
 
	AST_Node* ast_node = new(AST_Node);
	
	ast_node -> node = node;
	ast_node ->	nodeType = node_type;

	ast_node -> left = left;
	ast_node -> right = right;
	ast_node -> center = center;
	ast_node -> line = line;
	
	return ast_node;
}



// SPECIFIC NODE GENERATORS

AST_Node*
new_ast_expInteger_node ( int node, int node_type, int value, int line ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.ki = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, line);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expFloat_node ( int node, int node_type, float value, int line ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.kf = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, line);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expLiteral_node ( int node, int node_type, const char *value, int line ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);

	exp -> nextExpNode = NULL;
	exp -> u.lit = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, line);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expFuncCall_node ( int node, int node_type, Call *funcCall, int line ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.functionCall = funcCall;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, line);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expVariable_node( int node, int node_type, AST_Node *variableNode ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.varNode = variableNode;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, variableNode -> line);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_variable_node( int node, int node_type, const char *id, AST_Node *exp01, AST_Node *exp02, AST_Node *nextVarNode, int line ) {

	AST_Node *treeNode;
	Var *var = new(Var);
	
	var -> nextVarNode = nextVarNode;

	treeNode = new_ast_node(node, node_type, exp01, exp02, NULL, line);
	
	if(id != NULL)
		var -> varName = id;	
	
	treeNode -> nodeStruct.var = var;

	return treeNode;
}

AST_Node*
new_ast_type_node( int node, int node_type, const char *baseType, int line ) {

	AST_Node *treeNode;
	Type *type = new(Type);

	type -> baseType = baseType;
	type -> arraySequence = 0;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, line);
	
	treeNode -> nodeStruct.type = type;

	return treeNode;
}

AST_Node*
new_ast_defVariable_node( int node, int node_type, AST_Node* typeNode, AST_Node* varListNode ) {

	AST_Node *treeNode;
	AST_Node *temp;
	Def *def = new(Def);
	DefVar *defVar = new(DefVar);
	
	if(typeNode -> nodeStruct.type -> arraySequence > 0) { // Is an array
		
		for(temp = varListNode; temp != NULL; temp = temp -> nodeStruct.var -> nextVarNode)
			temp -> nodeType = VAR_INDEXED;
	}

	defVar -> dataTypeNode = typeNode;
	defVar -> varListNode = varListNode;
	def -> u.defVar = defVar;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, typeNode -> line);
	treeNode -> nodeStruct.def = def;

	return treeNode;
}

AST_Node*
new_func_def( const char* returnType, const char *funcName, Param *param, AST_Node *block, AST_Node *node, int line ) {
	
	AST_Node *funcNode = new_ast_node(DEF, DEF_FUNC, NULL, NULL, NULL, line);
	Def *def = new(Def);
	
	def -> u.func.block = block;
	def -> u.func.param = param;
	def -> u.func.funcName = funcName;
	
	if ( node == NULL ) {
		
		def -> u.func.tagReturnType = 0;
		def -> u.func.ret.voidType = returnType;	
	}
	else {
		
		def -> u.func.tagReturnType = 1;
		def -> u.func.ret.dataTypeNode = node;
			
	}
	
	funcNode -> nodeStruct.def = def;
	
	return funcNode;
}

AST_Node*
new_command_func_calling( Call *func, int line ) {
	
	AST_Node *funcNode = new_ast_node(STAT, STAT_FUNC_CALL, NULL, NULL, NULL, line);
	
	Stat *call = new(Stat);
	(call -> u).callFunc = func;
	
	(funcNode -> nodeStruct).stat = call;
	
	return funcNode;	
}


// STRUCT GENERATORS

Call*
new_funcCall( const char* id, AST_Node *expListNode ) {

	Call *funcCall = new(Call);
	
	funcCall -> funcName = id;
	funcCall -> expressionNode = expListNode;

	return funcCall;
}

Param*
new_param( AST_Node *type, const char *paramName, Param *nextParam) {
	
	AST_Node *varNode;
	Param *paramNode = new(Param);
	DefVar* defVar = new(DefVar);
	int varType = VAR_UNIQUE;
		
	if(type -> nodeStruct.type -> arraySequence > 0)
		varType = VAR_INDEXED;

	varNode = new_ast_variable_node(VAR, varType, paramName, NULL, NULL, NULL, type -> line);
	
	defVar -> dataTypeNode = type;
	defVar -> varListNode = varNode;
	
	paramNode -> proxParam = nextParam;
	paramNode -> var = defVar;
	
	return paramNode;
		
}




AST_Node*
new_stat_if( int i, int j, AST_Node* n1, AST_Node* n2, AST_Node* n3, int line) {
	
	AST_Node *node = new_ast_node(i, j, NULL, NULL, NULL, line);
	
	Stat *statIf = new(Stat);
	statIf -> u.ifCondition.exp00Node = n1;
	statIf -> u.ifCondition.block = n2;
	statIf -> u.ifCondition.elseNo = n3;
	
	node -> nodeStruct.stat = statIf;
	
	return node;
		
}


AST_Node*
new_stat_while( int i, int j, AST_Node* n1, AST_Node* n2, int line) {
	
	AST_Node *node = new_ast_node(i, j, NULL, NULL, NULL, line);
	
	Stat *statW = new(Stat);
	statW -> u.whileLoop.exp00Node = n1;
	statW -> u.whileLoop.commandListNode = n2;
	
	node -> nodeStruct.stat = statW;
	
	return node;		
}
	
AST_Node*
new_stat_assign( int i, int j, AST_Node* n1, AST_Node* n2, int line) {
	
	AST_Node *node = new_ast_node(i, j, NULL, NULL, NULL, line);
	
	Stat *statA = new(Stat);
	statA -> u.assign.exp00Node = n2;
	statA -> u.assign.varNode = n1;
		
	node -> nodeStruct.stat = statA;
	
	return node;		
		
}
	
	
AST_Node*
new_stat_ret( int i, int j, AST_Node* n1, int line) {
	
	AST_Node *node = new_ast_node(i, j, NULL, NULL, NULL, line);
	
	Stat *statR = new(Stat);
	statR -> u.retCommand.exp00Node = n1;
		
	node -> nodeStruct.stat = statR;
	
	return node;		
		
}












// OTHER FUNCTIONS

AST_Node*
connect_exp_list( AST_Node *father, AST_Node *son ) {

	father -> nodeStruct.exp -> nextExpNode = son; 
	return father;
}


AST_Node*
isArrayType( AST_Node *typeNode ) {

	typeNode -> nodeStruct.type -> arraySequence++;

	return typeNode;
}

static void* myMalloc( size_t size ) {
	
	void* obj = (void *)malloc(size);
	
	if(obj == NULL) { 	
		printf("MEMORY SHORTAGE ERROR\n"); 
		exit(0); 	
	}	
	return obj;
}

Param*
connect_param_list( Param *father, Param *son ) {
	
	father -> proxParam = son;	
	return father;
		
}

AST_Node*
connect_node_left(AST_Node *varDef, AST_Node *commandSeq) {
	
	if ( varDef == NULL)
		return commandSeq;
	
	varDef -> left = commandSeq;
	return varDef;	
}



AST_Node*
connect_node_right(AST_Node *varDef, AST_Node *commandSeq) {
	
	if ( varDef == NULL)
		return commandSeq;
	
	varDef -> right = commandSeq;
	return varDef;	
}




// PRINTING FUNCTIONS

static void print_type(AST_Node *a) {
	
	int i;
	
	if( a != NULL) {
		
		printf("TYPE: ");
		printf("%s\n", a -> nodeStruct.type -> baseType);	
				
		for(i = 0; i < a -> nodeStruct.type -> arraySequence ; i++) 
			printf("[]\n");
				
	}
		
}




static void print_exp (AST_Node *a) {
	
	if( a != NULL) {
		
		if ( a -> nodeType == EXPR_VAR) {
			
			printf("EXPR_VAR: ");		
			print_var( a -> nodeStruct.exp -> u.varNode);
			print_exp(a -> nodeStruct.exp->nextExpNode);		
		
		} else if (a -> nodeType == EXPR_FUNC_CALL ) {
			
			printf("EXPR_FUNC_CALL: ");
			print_call(a -> nodeStruct.exp -> u.functionCall);
			print_exp(a -> nodeStruct.exp->nextExpNode);
		
		}  else if (a -> nodeType == EXPR_NEW) {
			
			printf("EXPR_NEW: ");
			print_type(a->left);
			print_exp(a->right);
			
		}  else if (a -> nodeType == EXPR_INT) {
			
			printf("EXPR_INT: ");
			printf("%d\n", a -> nodeStruct.exp -> u.ki );
			print_exp(a -> nodeStruct.exp->nextExpNode);
		
		}  else if (a -> nodeType == EXPR_HEXA) {
			
			printf("EXPR_HEXA: ");
			printf("%x\n", a -> nodeStruct.exp -> u.ki );
			print_exp(a -> nodeStruct.exp->nextExpNode);
		
		}  else if (a -> nodeType == EXPR_CHAR ) {
			
			printf("EXPR_CHAR: ");
			printf("%c\n", a -> nodeStruct.exp -> u.ki );
			print_exp(a -> nodeStruct.exp->nextExpNode);
		
		}  else if (a -> nodeType == EXPR_FLOAT) {
			
			printf("EXPR_FLOAT: ");
			printf("%g\n", a -> nodeStruct.exp -> u.kf );
			print_exp(a -> nodeStruct.exp->nextExpNode);	
		
		}  else if (a -> nodeType == EXPR_LIT ) {
			
			printf("EXPR_LIT: ");
			printf("%s\n", a -> nodeStruct.exp -> u.lit );
			print_exp(a -> nodeStruct.exp->nextExpNode);
		
		} else { 
				
			if ( a -> nodeType == EXPR_OR ) {
			
				printf("EXPR_OR\n");
			
			
			} else if ( a -> nodeType == EXPR_AND) {
			
				printf("EXPR_AND\n");
			
			
			} else if ( a -> nodeType == EXPR_EQUAL ) {
			
				printf("EXPR_EQUAL\n");
			
			
			} else if ( a -> nodeType == EXPR_LEEQ ) {
			
				printf("EXPR_LEEQ\n");
			
			
			} else if ( a -> nodeType == EXPR_GREQ) {
			
				printf("EXPR_GREQ\n");
			
			
			} else if ( a -> nodeType == EXPR_GREATER) {
			
				printf("EXPR_GREATER\n");
			
			
			} else if ( a -> nodeType == EXPR_LESS) {
			
				printf("EXPR_LESS\n");
			
			
			} else if ( a -> nodeType == EXPR_NOEQ ) {
			
				printf("EXPR_NOEQ\n");
			
			
			
			} else if ( a -> nodeType == EXPR_ADD) {
			
				printf("EXPR_ADD\n");
			
			
			} else if ( a -> nodeType == EXPR_MIN) {
			
				printf("EXPR_MIN\n");
			
			
			} else if ( a -> nodeType == EXPR_MUL) {
			
				printf("EXPR_MUL\n");
			
			
			} else if ( a -> nodeType == EXPR_DIV) {
			
				printf("EXPR_DIV\n");
			
			
			} else if ( a -> nodeType == EXPR_NOT ) {
			
				printf("EXPR_NOT\n");
			
			
			} else {
			
				printf("EXPR_NEG\n");	
				
			}
		
			print_exp(a->left);
			print_exp(a->right);
						
		}					
	
		
	}
		
}



static void print_var(AST_Node *a) {
	
	if( a != NULL) {
				
		if ( a -> nodeType == VAR_UNIQUE ) {
			
			printf("VAR_UNIQUE: ");
			printf("%s\n",  a -> nodeStruct.var -> varName );
		}
		else {		
			
			printf("	VAR_INDEXED\n");
			print_exp(a->left);
			print_exp(a->right);		
		}
					
		print_var(a -> nodeStruct.var -> nextVarNode);
		print_tree(a->left);
		print_tree(a->right);		
	}
		
}



static void print_params(Param *p) {
	
	if( p != NULL) {
		
		printf("PARAM:\n");
		print_type(p -> var -> dataTypeNode);
		print_var(p -> var -> varListNode);
		print_params(p -> proxParam);			
	}

}





		

static void print_stat (AST_Node *a) {
	
	
	if( a != NULL) {
		
		if(  a -> nodeType == STAT_WHILE ) {
			
			printf("\nSTAT_WHILE\n");
			print_exp(a -> nodeStruct.stat -> u.whileLoop.exp00Node);
			print_stat(a -> nodeStruct.stat -> u.whileLoop.commandListNode);
			
		} else if (a -> nodeType == STAT_IF) {
		
			printf("\nSTAT_IF\n");
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block);
			
		
		} else if(  a -> nodeType == STAT_IFELSE) {
			
			printf("\nSTAT_IFELSE\n");
			
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo);	
		
		
		
		} else if(a -> nodeType == STAT_ASSIGN) {
			
			printf("\nSTAT_ASSIGN\n");
			
			print_var(a -> nodeStruct.stat -> u.assign.varNode);
			print_exp(a -> nodeStruct.stat -> u.assign.exp00Node);
			
			
		} else if(a -> nodeType == STAT_RETURN) {
			
			printf("\nSTAT_RETURN\n");
			print_exp(a -> nodeStruct.stat -> u.retCommand.exp00Node);
				
		} else {
		
			printf("\nSTAT_FUNC\n");
			print_call(a -> nodeStruct.stat -> u.callFunc);	
		}
	
		print_tree(a->right);
		print_tree(a->left);
		
	}	
}


static void print_call (Call *a) {
	
	if( a != NULL) {
		
		printf("FUNC_CALL: ");
		printf("%s\n", a -> funcName );
		print_exp(a -> expressionNode);
		
	}
	
}


static void print_block(AST_Node *a) {
	
	if( a != NULL) {
		
		printf("\n\tBLOCO");
		
		if ( a -> nodeType == DEF_VAR ) {
			
			print_def(a);
			printf("\n");
			print_stat(a->right);
		
		} else	
			print_stat(a);		
	}		
}



static void
print_def(AST_Node *a) {
	
	
	if( a != NULL ) {
	
	
		if(a -> nodeType == DEF_VAR) {
		
			printf("\nDEF_VAR\n");
			print_type( a -> nodeStruct.def -> u.defVar -> dataTypeNode );
			print_var(  a -> nodeStruct.def -> u.defVar -> varListNode );		
	
		} else {
		
			printf("\tDEF_FUNC:\n");
		
			if(a -> nodeStruct.def -> u.func.tagReturnType)
				print_type( a -> nodeStruct.def -> u.func.ret.dataTypeNode);
			else printf("void\n");
		
		
			printf("%s\n", a -> nodeStruct.def -> u.func.funcName );	
		
			print_params( a -> nodeStruct.def -> u.func.param );
			print_block( a -> nodeStruct.def -> u.func.block );
					
		}
		
		print_tree(a->left);
		
	}

}


void print_tree(AST_Node *a) {
	 
 	if( a!= NULL )
 	   switch( a -> node ) {
			case DEF:  print_def(a); break;
			case VAR:  print_var(a); break;
			case EXPR: print_exp(a); break;
			case STAT: print_stat(a); break;	   
			case TYPE: print_type(a); break;
	  
	   	 default: printf("internal error: bad node %c\n", a->nodeType);
		}  
}




