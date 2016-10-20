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


// OTHER FUNCTIONS

AST_Node*
connect_exp_list( AST_Node *father, AST_Node *son ) {

	father -> nodeStruct.exp -> nextExpNode = son;
	return father;
}

AST_Node*
connect_definitions( AST_Node *currentDef, AST_Node *nextDef ) {

	currentDef -> right = nextDef;
	return currentDef;
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



// PRINT AND MEMORY RELEASE FUNCTIONS



int main (void) {
	
	
}

