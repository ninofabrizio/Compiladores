#include "ast.h"

static void* myMalloc(size_t size);

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


// STRUCT GENERATORS

Call*
new_funcCall( const char* id, AST_Node *expListNode ) {

	Call *funcCall = new(Call);
	
	funcCall -> funcName = id;
	funcCall -> expressionNode = expListNode;

	return funcCall;
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

Param*
new_param( AST_Node *type, const char *paramName, Param *nextParam) {
	
	Param *paramNode = new(Param);
	DefVar* defVar = new(DefVar);
	Var* var = new(Var);
	
	var -> varName =  paramName;
	defVar -> dataTypeNode = type;
	defVar -> varListNode = var;
	
	paramNode -> proxParam = nextParam;
	paramNode -> var = defVar;
	
	return paramNode;
		
}


Param*
connect_param_list( Param *father, Param *son ) {
	
	father -> proxParam = son;	
	return father;
		
}

AST_Node*
connect_node(AST_Node *varDef, AST_Node *commandSeq) {
	
	if ( varDef == NULL)
		return commandSeq;
	
	varDef -> right = commandSeq;
	return varDef;	
}


AST_Node*
new_command_func_calling( Call *func, int line ) {
	
	AST_Node *funcNode = new_ast_node(STAT, STAT_FUNC_CALL, NULL, NULL, NULL, line);
	
	Stat *call = new(Stat);
	(call -> u).callFunc = func;
	
	(funcNode -> nodeStruct).stat = call;
	
	return funcNode;	
}




void print_type(AST_Node *a) {
	
	int i;
	
	if( a != NULL) {
		
		printf("%s\n", a -> nodeStruct.type -> baseType);	
				
		for(i = 0; i < a -> nodeStruct.type -> arraySequence ; i++) 
			printf("[]\n");
				
	}
		
}



void print_var(AST_Node *a) {
	
	if( a != NULL) {
		
		if ( a -> nodeType == VAR_UNIQUE )
			printf("%s\n",  a -> nodeStruct.var -> varName );
		
		else {		
			print_exp(a->left);
			print_exp(a->right);		
		}
					
		print_var(a -> nodeStruct.var -> nextVarNode);
				
	}
		
}



void print_params(Param *p) {
	
	if( p != NULL) {
		
		print_type(p -> var -> dataTypeNode);
		print_var(p -> var -> varListNode);
		print_params(p -> proxParam);			
	}
}



void print_block(AST_Node *a) {
	
	if( a != NULL) {
		
		if ( a -> nodeType == DEF_VAR ) {
			
			print_def(a);
			print_stat(a->right);
		
		} else	
			print_stat(a);		
	}		
}




void print_exp (AST_Node *a) {
	
	if( a != NULL){}
	
	
	
	
}


void print_stat (AST_Node *a) {
	
	
	
	
	
	
}







void
print_def(AST_Node *a) {
	
	
	if( a != NULL ) {
	
	
		if(a -> nodeType == DEF_VAR) {
		
			printf("DEF_VAR\n");
			print_type( a -> nodeStruct.def -> u.defVar -> dataTypeNode );
			print_var(  a -> nodeStruct.def -> u.defVar -> varListNode );		
	
		} else {
		
			printf("DEF_FUNC\n");
		
			printf("%s\n", a -> nodeStruct.def -> u.func.funcName );	
		
			if(a -> nodeStruct.def -> u.func.tagReturnType)
				print_type( a -> nodeStruct.def -> u.func -> dataTypeNode);
			else printf("void\n");
		
			print_params( a -> nodeStruct.def -> u.func.param );
			print_block( a -> nodeStruct.def -> u.func.block );
					
		}
		
	}

}






void
print_tree(AST_Node *a) {
 
 	if( a!= NULL )
 	   switch( a -> node ) {
			case DEF:  print_def(a); break;
			case VAR:  print_var(a); break;
			case EXPR: print_expr(a); break;
			case STAT: print_stat(a); break;	   
			case TYPE: print_type(a); break;
	  
	   	 default: printf("internal error: bad node %c\n", a->nodeType);
		}  
}











