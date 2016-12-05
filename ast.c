#include "ast.h"

static void* myMalloc(size_t size);
static void print_def(AST_Node *a, int tabIndex);
static void print_block(AST_Node *a, int tabIndex);
static void print_call (Call *a, int tabIndex);
static void print_stat (AST_Node *a, int tabIndex);
static void print_type(AST_Node *a, int tabIndex);
static void print_var(AST_Node *a, int tabIndex);

symbol_table* symbol_table_create (void);
void symbol_table_destroy (symbol_table* table);
id_entry* symbol_table_find_entry (symbol_table *table, const char *name);
symbol_table* insert (symbol_table *table, const char *name, void* nodeRef, boolean *present);
Stack* single_table_create (void);
void single_table_destroy (Stack *single_table);
id_entry* single_table_find (Stack *single_table, const char *elemFound);
id_entry* single_table_find_current_scope (Stack *single_table, const char *elemFound);
void single_table_insert_current_scope (Stack *single_table, const char *name, void* nodeRef, boolean *present);
symbol_table* single_table_pop_scope (Stack *single_table);
void single_table_push_scope (Stack *single_table, symbol_table *newTable);

extern Stack *single_table; /* tabela de simbolos sendo uma pilha de escopos */

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
	exp -> typing = NULL;

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
	exp -> typing = NULL;

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
	exp -> typing = NULL;

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
	exp -> typing = NULL;

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
	exp -> typing = NULL;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, variableNode -> line);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expNode (int node, int node_type, AST_Node *left, AST_Node *right, AST_Node *center, int line ) {

	AST_Node *treeNode = new_ast_node(node, node_type, left, right, center, line);
	Exp *exp = new(Exp);

	exp -> nextExpNode = NULL;
	exp -> typing = NULL;
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
	var -> typing = NULL;
	var -> linkedVarNode = NULL;

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
	Def *def = new(Def);
	DefVar *defVar = new(DefVar);

	defVar -> dataTypeNode = typeNode;
	defVar -> varListNode = varListNode;
	def -> u.defVar = defVar;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL, typeNode -> line);
	treeNode -> nodeStruct.def = def;

	return treeNode;
}

AST_Node*
new_func_def( const char* returnVoid, const char *funcName, Param *param, AST_Node *block, AST_Node *nodeType, int line ) {
	
	AST_Node *funcNode;
	Def *def = new(Def);

	// Just so we try to get the nearest line number of where the actual definition is
	if(nodeType != NULL)
		funcNode = new_ast_node(DEF, DEF_FUNC, NULL, NULL, NULL, nodeType -> line);
	else if(param != NULL)
		funcNode = new_ast_node(DEF, DEF_FUNC, NULL, NULL, NULL, param -> var -> dataTypeNode -> line);
	else
		funcNode = new_ast_node(DEF, DEF_FUNC, NULL, NULL, NULL, line);
	
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
	
	AST_Node *funcNode;

	// Just so we try to get the nearest line number of where the actual function calling is
	if(func -> expressionNode != NULL )
		funcNode = new_ast_node(STAT, STAT_FUNC_CALL, NULL, NULL, NULL, func -> expressionNode -> line);
	else
		funcNode = new_ast_node(STAT, STAT_FUNC_CALL, NULL, NULL, NULL, line);
	
	Stat *Statcall = new(Stat);
	Statcall -> u.callFunc = func;
	Statcall -> typing = NULL;
	
	funcNode -> nodeStruct.stat = Statcall;
	
	return funcNode;	
}


Call*
new_funcCall( const char* id, AST_Node *expListNode ) {

	Call *funcCall = new(Call);
	
	funcCall -> funcName = id;
	funcCall -> expressionNode = expListNode;
	funcCall -> linkedFuncNode = NULL;

	return funcCall;
}

Param*
new_param( AST_Node *type, const char *paramName, Param *nextParam) {
	
	AST_Node *varNode;
	Param *paramNode = new(Param);
	DefVar* defVar = new(DefVar);
	int varType = VAR_UNIQUE;

	varNode = new_ast_variable_node(VAR, varType, paramName, NULL, NULL, NULL, type -> line);
	
	defVar -> dataTypeNode = type;
	defVar -> varListNode = varNode;
	
	paramNode -> proxParam = nextParam;
	paramNode -> var = defVar;
	
	return paramNode;
		
}

AST_Node*
new_stat_if( int node, int nodeType, AST_Node* n1, AST_Node* n2, AST_Node* n3 ) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, n1 -> line);
	
	Stat *statIf = new(Stat);
	statIf -> u.ifCondition.exp00Node = n1;
	statIf -> u.ifCondition.block = n2;
	statIf -> u.ifCondition.elseNo = n3;
	statIf -> typing = NULL;
	
	treeNode -> nodeStruct.stat = statIf;
	
	return treeNode;
		
}


AST_Node*
new_stat_while( int node, int nodeType, AST_Node* n1, AST_Node* n2 ) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, n1 -> line);
	
	Stat *statW = new(Stat);
	statW -> u.whileLoop.exp00Node = n1;
	statW -> u.whileLoop.commandListNode = n2;
	statW -> typing = NULL;
	
	treeNode -> nodeStruct.stat = statW;
	
	return treeNode;		
}
	
AST_Node*
new_stat_assign( int node, int nodeType, AST_Node* n1, AST_Node* n2, int line) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, line);
	
	Stat *statA = new(Stat);
	statA -> u.assign.varNode = n1;
	statA -> u.assign.exp00Node = n2;
	statA -> typing = NULL;
		
	treeNode -> nodeStruct.stat = statA;
	
	return treeNode;		
		
}
	
	
AST_Node*
new_stat_ret( int node, int nodeType, AST_Node* n1, int line) {
	
	AST_Node *treeNode = new_ast_node(node, nodeType, NULL, NULL, NULL, line);
	
	Stat *statR = new(Stat);
	statR -> u.retCommand.exp00Node = n1;
	statR -> typing = NULL;
		
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
			
			printf("Line [%d] EXPR_VAR {\n", a -> line);	
			print_var(a -> nodeStruct.exp -> u.varNode, tabIndex+1);	
			free(a -> nodeStruct.exp -> u.varNode);

		} else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			printf("Line [%d] EXPR_FUNC_CALL {\n", a -> line);
			print_call(a -> nodeStruct.exp -> u.functionCall, tabIndex+1);

		}  else if ( a -> nodeType == EXPR_NEW ) {
			
			printf("Line [%d] EXPR_NEW {\n", a -> line);
			print_type(a->left, tabIndex+1);
			print_exp(a->right, tabIndex+1);
			a->left = NULL;
			a->right = NULL;
			
		}  else if ( a -> nodeType == EXPR_INT ) {
			
			printf("Line [%d] EXPR_INT {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %d\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_HEXA ) {
			
			printf("Line [%d] EXPR_HEXA {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %#x\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_CHAR ) {
			
			printf("Line [%d] EXPR_CHAR {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %c\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_FLOAT ) {
			
			printf("Line [%d] EXPR_FLOAT {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %f\n", a -> nodeStruct.exp -> u.kf);
			
		}  else if ( a -> nodeType == EXPR_LIT ) {
			
			printf("Line [%d] EXPR_LIT {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: \"%s\"", a -> nodeStruct.exp -> u.lit);
			
		} else if ( a -> nodeType == EXPR_OR ) {
			
			printf("Line [%d] EXPR_OR {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_AND ) {
			
			printf("Line [%d] EXPR_AND {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_EQUAL ) {
			
			printf("Line [%d] EXPR_EQUAL {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_LEEQ ) {
			
			printf("Line [%d] EXPR_LEEQ {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_GREQ ) {
			
			printf("Line [%d] EXPR_GREQ {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_GREATER ) {
			
			printf("Line [%d] EXPR_GREATER {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_LESS ) {
			
			printf("Line [%d] EXPR_LESS {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_NOEQ ) {
			
			printf("Line [%d] EXPR_NOEQ {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_ADD ) {
			
			printf("Line [%d] EXPR_ADD {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_MIN ) {
			
			printf("Line [%d] EXPR_MIN {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_MUL ) {
			
			printf("Line [%d] EXPR_MUL {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_DIV ) {
			
			printf("Line [%d] EXPR_DIV {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_NOT ) {
			
			printf("Line [%d] EXPR_NOT {\n", a -> line);
			
		} else {
			
			printf("Line [%d] EXPR_NEG {\n", a -> line);
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

			// GAMBIARRA: A way to not print NULL value for the name
			/*if(a -> nodeStruct.var -> varName != NULL) {
				//printf("\n");
				for(count = 0; count < tabIndex+1; count++)
					printf("\t");

				printf("VAR NAME: %s\n", a -> nodeStruct.var -> varName);
			}*/
		
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
			
			printf("Line [%d] STAT_WHILE {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.whileLoop.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.whileLoop.commandListNode, tabIndex+1);
				
		} else if ( a -> nodeType == STAT_IF ) {
		
			printf("Line [%d] STAT_IF {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block, tabIndex+1);
		
		} else if( a -> nodeType == STAT_IFELSE ) {
			
			printf("Line [%d] STAT_IFELSE {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo, tabIndex+1);	
		
		} else if( a -> nodeType == STAT_ASSIGN ) {
			
			printf("Line [%d] STAT_ASSIGN {\n", a -> line);
			print_var(a -> nodeStruct.stat -> u.assign.varNode, tabIndex+1);
			print_exp(a -> nodeStruct.stat -> u.assign.exp00Node, tabIndex+1);
			free(a -> nodeStruct.stat -> u.assign.varNode);
			
		} else if( a -> nodeType == STAT_RETURN ) {
			
			printf("Line [%d] STAT_RETURN {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.retCommand.exp00Node, tabIndex+1);
		
		} else {
		
			printf("Line [%d] STAT_FUNC {\n", a -> line);
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

			printf("Line [%d] DEF_VAR {\n", a -> line);
			
			print_type(a -> nodeStruct.def -> u.defVar -> dataTypeNode, tabIndex+1);
			print_var(a -> nodeStruct.def -> u.defVar -> varListNode, tabIndex+1);
				
	
			//if( a -> nodeStruct.def -> u.defVar -> varListNode -> nodeType == VAR_INDEXED )
			//	printf("%s", a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> varName);

			free(a -> nodeStruct.def -> u.defVar -> varListNode);
			free(a -> nodeStruct.def -> u.defVar);

		} else  {
		
			printf("Line [%d] DEF_FUNC {\n", a -> line);

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
			print_params(a -> nodeStruct.def -> u.func.param, tabIndex + 1);
			print_block(a -> nodeStruct.def -> u.func.block, tabIndex + 1);
				
		}
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

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
	   	 	default: fprintf(stderr, "AST PRINTING ERROR: UNKNOWN NODE \"%c\"\n", a->nodeType); 
			exit(0);
	 	}
}




void 
build_single_table (AST_Node *root) 
{

	boolean present;	
	const char *type = NULL;
	Param *param = NULL;
	AST_Node *var = NULL;
	
	
	if ( root != NULL ) {
	
		if ( root -> node == DEF ) {
	
			if ( root -> nodeType == DEF_VAR )
		
				for ( var = root -> nodeStruct.def -> u.defVar -> varListNode; var != NULL; var = var -> nodeStruct.var -> nextVarNode ) 
			
					single_table_insert_current_scope (single_table,
													   var -> nodeStruct.var -> varName, 
													   root, 
													   &present);			
			else {
					
				
				if (single_table -> prim != NULL) {				
					
					symbol_table *closeScope;
				
					closeScope = single_table_pop_scope (single_table); /* close scope before */
								
				}
				
				single_table_push_scope ( single_table, symbol_table_create ( ) );
					
				single_table_insert_current_scope (single_table, 
												   root -> nodeStruct.def -> u.func.funcName, 
												   root, 
												   &present);			
				
				
				
				single_table_push_scope ( single_table, symbol_table_create ( ) );
				
				/* List of function parameters, if they exist */
			    if ( root -> nodeStruct.def -> u.func.param != NULL ) {
					
					id_entry* entry = NULL;
					
					for ( param = ( root -> nodeStruct.def -> u.func.param ); param != NULL; param = param -> proxParam )

						single_table_insert_current_scope (single_table,
														   param -> var -> varListNode -> nodeStruct.var -> varName, 
														   param -> var -> varListNode,
														   &present);
				
						
						
			 }
														   
		        build_single_table( root -> nodeStruct.def -> u.func.block );		
		  	}
					
		} else if ( root -> node == STAT ) {
			
			
			if ( root -> nodeType == STAT_IF ) {
				
				single_table_push_scope ( single_table, symbol_table_create ( ) );
				build_single_table( root -> nodeStruct.stat -> u.ifCondition.exp00Node );
				build_single_table( root -> nodeStruct.stat -> u.ifCondition.block );
				symbol_table *closeScope = single_table_pop_scope (single_table); /* closing if scope */
								
			} else if ( root -> nodeType == STAT_WHILE ) {
						
				single_table_push_scope ( single_table, symbol_table_create ( ) );
				build_single_table( root -> nodeStruct.stat -> u.whileLoop.exp00Node );	
				build_single_table( root -> nodeStruct.stat -> u.whileLoop.commandListNode );
				symbol_table *closeScope = single_table_pop_scope (single_table); /* closing while scope */
					
			} else if ( root -> nodeType == STAT_IFELSE ) {
				
				symbol_table *closeScope = NULL;
				
				single_table_push_scope ( single_table, symbol_table_create ( ) );
				build_single_table( root -> nodeStruct.stat -> u.ifCondition.exp00Node );
				build_single_table( root -> nodeStruct.stat -> u.ifCondition.block );			
				closeScope = single_table_pop_scope (single_table); /* closing if scope */
				
				single_table_push_scope ( single_table, symbol_table_create ( ) );
				build_single_table( root -> nodeStruct.stat -> u.ifCondition.elseNo );
				closeScope = single_table_pop_scope (single_table); /* closing else scope */	
	
			
			} else if( root -> nodeType == STAT_ASSIGN ) {
					
				build_single_table( root -> nodeStruct.stat -> u.assign.varNode );
				build_single_table( root -> nodeStruct.stat -> u.assign.exp00Node);
						
			} else if( root -> nodeType == STAT_RETURN ) {
				
				build_single_table( root -> nodeStruct.stat -> u.retCommand.exp00Node);
					
			} else {
				
					id_entry* elem = NULL;
			
					AST_Node *test = NULL;
					
					elem = single_table_find ( single_table, 
										     ( root -> nodeStruct.stat -> u.callFunc -> funcName) );
				
					if ( elem == NULL ) {
						printf( "\n\n**ERROR: function \"%s\" not declared**", (root -> nodeStruct.stat -> u.callFunc -> funcName) );
						exit(0);
				
					}
				
					 //else
 						//printf( "\n....Costurou a function: %s....\n", (root -> nodeStruct.stat -> u.callFunc -> funcName) );

					root -> nodeStruct.stat -> u.callFunc -> linkedFuncNode = (AST_Node *)(elem -> nodeRef);
					
					
					//test = root -> nodeStruct.stat -> u.callFunc -> linkedFuncNode;
				
				
					//printf( "\n**Func: %s **\n", (test -> nodeStruct.def -> u.func.funcName) );

					build_single_table(root -> nodeStruct.stat -> u.callFunc -> expressionNode);
			}
					
		
		} else if ( root -> node == VAR ) {
			
				id_entry* elem = NULL;
				
				AST_Node *test = NULL;
			
				elem = single_table_find_current_scope ( single_table, 
			 										   ( root -> nodeStruct.var -> varName) );
			
				
				if ( elem == NULL ) {
					
					elem = single_table_find ( single_table, 
											 ( root -> nodeStruct.var -> varName) );
					
					
					if ( elem == NULL ) {
						printf( "\n\n**ERROR: Variable \"%s\" not declared**\n\n", root -> nodeStruct.var -> varName);
						exit(0);
					}
				
				}
				
				if( ((AST_Node *)(elem -> nodeRef)) -> nodeType == DEF_FUNC ) {
					printf( "\n\n**ERROR: Variable \"%s\" has been connected to a function**\n\n", root -> nodeStruct.var -> varName);
					exit(0);
				}

				//printf( "\n....Costurou a var: %s....\n", (root -> nodeStruct.var -> varName) );
			
				
				root -> nodeStruct.var -> linkedVarNode = (AST_Node *)(elem -> nodeRef);
				
				////////////////TESTE.....///////////////////
				
				//test = root -> nodeStruct.var -> linkedVarNode;
				
				 //if ( test -> node == DEF)
 				//	printf( "\n**Variable: %s **\n", (test -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> varName) );

 				//else if (test -> node == VAR)
 				//	printf( "\n**Variable: %s **\n", (test -> nodeStruct.var -> varName) );

				
		
		
		
				////////////////////////////////////////////
		
		
		
			
		} else if ( root -> node == EXPR ) {
			
			AST_Node *test = NULL;
		
			if ( root -> nodeType == EXPR_VAR ) {
				
				build_single_table(root -> nodeStruct.exp -> u.varNode);	
				
			} else if ( root -> nodeType == EXPR_FUNC_CALL ) {
			
				id_entry* elem = NULL;
			
				elem = single_table_find ( single_table, 
									     ( root -> nodeStruct.exp -> u.functionCall -> funcName) );
				
				if ( elem == NULL ){
					printf( "\n\n**ERROR: function \"%s\" not declared**", (root -> nodeStruct.exp -> u.functionCall -> funcName) );
					exit(0);
				}
				//else
				//	printf( "\n....Costurou a function: %s....\n", (root -> nodeStruct.exp -> u.functionCall -> funcName) );
			
				
				root -> nodeStruct.exp -> u.functionCall -> linkedFuncNode = (AST_Node *)(elem -> nodeRef);
			
				////////////////////////TESTE/////////////////////////////////
				//test = root -> nodeStruct.exp -> u.functionCall -> linkedFuncNode;
				
				
				//printf( "\n**Func: %s **\n", (test -> nodeStruct.def -> u.func.funcName) );
		
				
				
				
				
				/////////////////////////////////////////////////////////////
				
				build_single_table(root -> nodeStruct.exp -> u.functionCall -> expressionNode);
				
			}  else if ( root -> nodeType == EXPR_NEW ) {
			
				build_single_table(root->right);
				root->left = NULL;
				root->right = NULL;
			
			}  else if ( root -> nodeType == EXPR_INT ) {
			
			}  else if ( root -> nodeType == EXPR_HEXA ) {
			
			}  else if ( root -> nodeType == EXPR_CHAR ) {		

			}  else if ( root -> nodeType == EXPR_FLOAT ) {
						
			}  else if ( root -> nodeType == EXPR_LIT ) {
					
			} else if ( root -> nodeType == EXPR_OR ) {
			
			} else if ( root -> nodeType == EXPR_AND ) {
			
			} else if ( root -> nodeType == EXPR_EQUAL ) {
			
			} else if ( root -> nodeType == EXPR_LEEQ ) {
			
			} else if ( root -> nodeType == EXPR_GREQ ) {
			
			} else if ( root -> nodeType == EXPR_GREATER ) {
	
			} else if ( root -> nodeType == EXPR_LESS ) {
			
			} else if ( root -> nodeType == EXPR_NOEQ ) {
			
			} else if ( root -> nodeType == EXPR_ADD ) {
			
			} else if ( root -> nodeType == EXPR_MIN ) {
			
			} else if ( root -> nodeType == EXPR_MUL ) {
			
			} else if ( root -> nodeType == EXPR_DIV ) {
			
			} else if ( root -> nodeType == EXPR_NOT ) {
			
			} else {

			}
		
			build_single_table(root -> left);
			build_single_table(root -> right);		
			build_single_table(root -> nodeStruct.exp -> nextExpNode);
		
		}
				
		build_single_table( root -> left  );
		build_single_table( root -> right );
		build_single_table( root -> center );	
	
	}

	//single_table_destroy(single_table);
}	
	


void 
print_single_table ( Stack *mySingleTable ) 
{
	
	// symbol_table *scope;
	// id_entry *elem;
	// int scopeNum = 0;
	//
	// printf("\n\nOPEN_SCOPES_STACK( TOP -> DOWN ):");
	//
	// while ( !stack_empty (mySingleTable) ) {
	//
	// 	scope = single_table_pop_scope (mySingleTable);
	// 	scopeNum += 1;
	//
	// 	printf( "\n\n_____________________OPEN_SCOPE - %d\n", scopeNum );
	//
	// 	for ( elem = scope -> prim; elem != NULL; elem = elem -> prox )
	//
	// 		printf( "\nisArray: %s\nType: %s\nName: %s\n", ( elem -> isArray )? "true"  :  "false" , elem -> type, elem -> name_id );
	//
	// 	printf( "____________________CLOSE_SCOPE - %d\n", scopeNum );
	//
	// }
	//
	// printf("\n\nCLOSE_SCOPES_STACK!!\n\n");
	//
}