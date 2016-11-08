#include "ast.h"

static void* myMalloc(size_t size);
static void print_def(AST_Node *a, int tabIndex);
static void print_block(AST_Node *a, int tabIndex);
static void print_call (Call *a, int tabIndex);
static void print_stat (AST_Node *a, int tabIndex);
static void print_type(AST_Node *a, int tabIndex);
static void print_var(AST_Node *a, int tabIndex);

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
new_ast_expNode (int node, int node_type, AST_Node *left, AST_Node *right, AST_Node *center, int line ) {

	AST_Node *treeNode = new_ast_node(node, node_type, left, right, center, line);
	Exp *exp = new(Exp);

	exp -> nextExpNode = NULL;
	treeNode -> nodeStruct.exp = exp;

	// Turning value to actual negative
	if(treeNode -> nodeType == EXPR_NEG) {
		if(treeNode -> left -> nodeType == EXPR_INT)
			treeNode -> left -> nodeStruct.exp -> u.ki = -treeNode -> left -> nodeStruct.exp -> u.ki;
		else if(treeNode -> left -> nodeType == EXPR_FLOAT)
			treeNode -> left -> nodeStruct.exp -> u.kf = -treeNode -> left -> nodeStruct.exp -> u.kf;
	}

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
new_func_def( const char* returnVoid, const char *funcName, Param *param, AST_Node *block, AST_Node *nodeType, int line ) {
	
	AST_Node *funcNode = new_ast_node(DEF, DEF_FUNC, NULL, NULL, NULL, line);
	Def *def = new(Def);
	
	def -> u.func.block = block;
	def -> u.func.param = param;
	def -> u.func.funcName = funcName;
	
	if ( nodeType == NULL ) {
		
		def -> u.func.tagReturnType = 0;
		def -> u.func.ret.voidType = returnVoid;	
	}
	else {
		
		def -> u.func.tagReturnType = 1;
		def -> u.func.ret.dataTypeNode = nodeType;
	}
	
	funcNode -> nodeStruct.def = def;
	
	return funcNode;
}

AST_Node*
new_command_func_calling( Call *func, int line ) {
	
	AST_Node *funcNode = new_ast_node(STAT, STAT_FUNC_CALL, NULL, NULL, NULL, line);
	
	Stat *Statcall = new(Stat);
	Statcall -> u.callFunc = func;
	
	funcNode -> nodeStruct.stat = Statcall;
	
	return funcNode;	
}


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
new_stat_if( int node, int nodeType, AST_Node* n1, AST_Node* n2, AST_Node* n3, int line) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, line);
	
	Stat *statIf = new(Stat);
	statIf -> u.ifCondition.exp00Node = n1;
	statIf -> u.ifCondition.block = n2;
	statIf -> u.ifCondition.elseNo = n3;
	
	treeNode -> nodeStruct.stat = statIf;
	
	return treeNode;
		
}


AST_Node*
new_stat_while( int node, int nodeType, AST_Node* n1, AST_Node* n2, int line) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, line);
	
	Stat *statW = new(Stat);
	statW -> u.whileLoop.exp00Node = n1;
	statW -> u.whileLoop.commandListNode = n2;
	
	treeNode -> nodeStruct.stat = statW;
	
	return treeNode;		
}
	
AST_Node*
new_stat_assign( int node, int nodeType, AST_Node* n1, AST_Node* n2, int line) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, line);
	
	Stat *statA = new(Stat);
	statA -> u.assign.varNode = n1;
	statA -> u.assign.exp00Node = n2;
		
	treeNode -> nodeStruct.stat = statA;
	
	return treeNode;		
		
}
	
	
AST_Node*
new_stat_ret( int node, int nodeType, AST_Node* n1, int line) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, line);
	
	Stat *statR = new(Stat);
	statR -> u.retCommand.exp00Node = n1;
		
	treeNode -> nodeStruct.stat = statR;
	
	return treeNode;
		
}


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
connect_node_left(AST_Node *node1, AST_Node *node2) {
	
	if ( node1 == NULL)
		return node2;
	
	node1 -> left = node2;
	return node1;	
}


AST_Node*
connect_node_right(AST_Node *node1, AST_Node *node2) {
	
	if ( node1 == NULL)
		return node2;
	
	node1 -> right = node2;
	return node1;	
}


static void
print_type ( AST_Node *a, int tabIndex ) {
	
	int i, count;
	
	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("TYPE: ");
		printf("%s", a -> nodeStruct.type -> baseType);	
				
		for(i = 0; i < a -> nodeStruct.type -> arraySequence ; i++) 
			printf("[]");
		printf("\n");

		free(a -> nodeStruct.type);		
		free(a);
	}	
}


static void
print_exp (AST_Node *a, int tabIndex) {
	
	int count;

	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		if ( a -> nodeType == EXPR_VAR ) {
			
			printf("EXPR_VAR {\n");	
			print_var(a -> nodeStruct.exp -> u.varNode, tabIndex+1);	
			free(a -> nodeStruct.exp -> u.varNode);

		} else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			printf("EXPR_FUNC_CALL {\n");
			print_call(a -> nodeStruct.exp -> u.functionCall, tabIndex+1);

		}  else if ( a -> nodeType == EXPR_NEW ) {
			
			printf("EXPR_NEW {\n");
			print_type(a->left, tabIndex+1);
			print_exp(a->right, tabIndex+1);
			a->left = NULL;
			a->right = NULL;
			
		}  else if ( a -> nodeType == EXPR_INT ) {
			
			printf("EXPR_INT {\n");

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %d\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_HEXA ) {
			
			printf("EXPR_HEXA {\n");

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %x\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_CHAR ) {
			
			printf("EXPR_CHAR {\n");

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %c\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_FLOAT ) {
			
			printf("EXPR_FLOAT {\n");

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %f\n", a -> nodeStruct.exp -> u.kf);
			
		}  else if ( a -> nodeType == EXPR_LIT ) {
			
			printf("EXPR_LIT {\n");

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %s\n", a -> nodeStruct.exp -> u.lit);
			
		} else if ( a -> nodeType == EXPR_OR ) {
			
			printf("EXPR_OR {\n");
			
		} else if ( a -> nodeType == EXPR_AND ) {
			
			printf("EXPR_AND {\n");
			
		} else if ( a -> nodeType == EXPR_EQUAL ) {
			
			printf("EXPR_EQUAL {\n");
			
		} else if ( a -> nodeType == EXPR_LEEQ ) {
			
			printf("EXPR_LEEQ {\n");
			
		} else if ( a -> nodeType == EXPR_GREQ ) {
			
			printf("EXPR_GREQ {\n");
			
		} else if ( a -> nodeType == EXPR_GREATER ) {
			
			printf("EXPR_GREATER {\n");
			
		} else if ( a -> nodeType == EXPR_LESS ) {
			
			printf("EXPR_LESS {\n");
			
		} else if ( a -> nodeType == EXPR_NOEQ ) {
			
			printf("EXPR_NOEQ {\n");
			
		} else if ( a -> nodeType == EXPR_ADD ) {
			
			printf("EXPR_ADD {\n");
			
		} else if ( a -> nodeType == EXPR_MIN ) {
			
			printf("EXPR_MIN {\n");
			
		} else if ( a -> nodeType == EXPR_MUL ) {
			
			printf("EXPR_MUL {\n");
			
		} else if ( a -> nodeType == EXPR_DIV ) {
			
			printf("EXPR_DIV {\n");
			
		} else if ( a -> nodeType == EXPR_NOT ) {
			
			printf("EXPR_NOT {\n");
			
		} else {
			
			printf("EXPR_NEG {\n");	
		}
		
		print_exp(a->left, tabIndex+1);
		print_exp(a->right, tabIndex+1);		

		print_exp(a -> nodeStruct.exp -> nextExpNode, tabIndex+1);	
		free(a -> nodeStruct.exp);

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		free(a);
	}	
}


static void
print_var( AST_Node *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
				
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		if ( a -> nodeType == VAR_UNIQUE ) {
			
			printf("VAR_UNIQUE {\n");

			//printf("\n");
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VAR NAME: %s\n", a -> nodeStruct.var -> varName);

		}
		else {		
			
			printf("VAR_INDEXED {\n");

			//printf("\n");
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VAR NAME: %s\n", a -> nodeStruct.var -> varName);
		
			print_exp(a->left, tabIndex+1);
			print_exp(a->right, tabIndex+1);
		
			a->left = NULL;
			a->right = NULL;
		
		}
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_var(a -> nodeStruct.var -> nextVarNode, tabIndex);
		
		print_tree(a->left, tabIndex);
		print_tree(a->right, tabIndex);

		free(a -> nodeStruct.var);
	}
}


static void
print_params ( Param *p, int tabIndex ) {
	
	int count;

	if( p != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("PARAM {\n");
		print_type(p -> var -> dataTypeNode, tabIndex+1);
		print_var(p -> var -> varListNode, tabIndex+1);
		
		//if ( p -> var -> varListNode -> nodeType == VAR_INDEXED)
		//	printf("%s\n", p -> var -> varListNode -> nodeStruct.var -> varName);
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_params(p -> proxParam, tabIndex);			
	
		free(p -> var -> varListNode);
		free(p -> var);
		free(p);
	}
}


static void
print_stat ( AST_Node *a, int tabIndex ) {
	
	int count;
	
	if( a != NULL ) {

		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		
		if( a -> nodeType == STAT_WHILE ) {
			
			printf("STAT_WHILE {\n");
			print_exp(a -> nodeStruct.stat -> u.whileLoop.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.whileLoop.commandListNode, tabIndex+1);
				
		} else if ( a -> nodeType == STAT_IF ) {
		
			printf("STAT_IF {\n");
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block, tabIndex+1);
		
		} else if( a -> nodeType == STAT_IFELSE ) {
			
			printf("STAT_IFELSE {\n");
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo, tabIndex+1);	
		
		} else if( a -> nodeType == STAT_ASSIGN ) {
			
			printf("STAT_ASSIGN {\n");
			print_var(a -> nodeStruct.stat -> u.assign.varNode, tabIndex+1);
			print_exp(a -> nodeStruct.stat -> u.assign.exp00Node, tabIndex+1);
			free(a -> nodeStruct.stat -> u.assign.varNode);
			
		} else if( a -> nodeType == STAT_RETURN ) {
			
			printf("STAT_RETURN {\n");
			print_exp(a -> nodeStruct.stat -> u.retCommand.exp00Node, tabIndex+1);
		
		} else {
		
			printf("STAT_FUNC {\n");
			print_call(a -> nodeStruct.stat -> u.callFunc, tabIndex+1);	
		}
	
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_tree(a->right, tabIndex);
		print_tree(a->left, tabIndex);
		
		free(a);
	}	
}


static void
print_call ( Call *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("FUNC_CALL {\n");

		for(count = 0; count < tabIndex+1; count++)
			printf("\t");
		
		printf("FUNC NAME: %s\n", a -> funcName);

		for(count = 0; count < tabIndex+1; count++)
			printf("\t");
		printf("PARAMETERS {\n");

		print_exp(a -> expressionNode, tabIndex+2);

		for(count = 0; count < tabIndex+1; count++)
			printf("\t");
		printf("}\n");

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");
	}

	free(a);
}


static void
print_block( AST_Node *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("BLOCK {\n");
		
		if ( a -> nodeType == DEF_VAR ) {
			
			print_def(a, tabIndex+1);
			//printf("\n");
			print_stat(a->right, tabIndex+1);

		} else	
			print_stat(a, tabIndex+1);

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");
	}			
}


static void
print_def ( AST_Node *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
	
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		
		if( a -> nodeType == DEF_VAR ) {

			printf("DEF_VAR {\n");
			
			print_type(a -> nodeStruct.def -> u.defVar -> dataTypeNode, tabIndex+1);
			print_var(a -> nodeStruct.def -> u.defVar -> varListNode, tabIndex+1);		
	
			//if( a -> nodeStruct.def -> u.defVar -> varListNode -> nodeType == VAR_INDEXED )
			//	printf("%s", a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> varName);

			free(a -> nodeStruct.def -> u.defVar -> varListNode);
			free(a -> nodeStruct.def -> u.defVar);

		} else  {
		
			printf("DEF_FUNC {\n");

			if(a -> nodeStruct.def -> u.func.tagReturnType == 1)
				print_type(a -> nodeStruct.def -> u.func.ret.dataTypeNode, tabIndex+1);
			else {

				for(count = 0; count < tabIndex+1; count++)
					printf("\t");

				printf("TYPE: void\n");
			}
		
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");
		
			printf("FUNC NAME: %s\n", a -> nodeStruct.def -> u.func.funcName);
			print_params(a -> nodeStruct.def -> u.func.param, tabIndex+1);
			print_block(a -> nodeStruct.def -> u.func.block, tabIndex+1);
				
		}
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n\n");

		print_tree(a->left, tabIndex);
		//print_tree(a->right, tabIndex);
	
		free(a -> nodeStruct.def);
		free(a);
	}
}


void
print_tree ( AST_Node *a, int tabIndex ) {
	 
 	if( a != NULL )
 	   switch( a -> node ) {
			case DEF:  print_def(a, tabIndex); break;
			case VAR:  print_var(a, tabIndex); break;
			case EXPR: print_exp(a, tabIndex); break;
			case STAT: print_stat(a, tabIndex); break;	   
			case TYPE: print_type(a, tabIndex); break;
	   	 	default: fprintf(stderr, "internal error: bad node %c\n", a->nodeType); exit(0);
	 	}
}