#include "ast.h"

static void* myMalloc(size_t size);

#define new(T) ((T*)myMalloc(sizeof(T)))


// GENERIC NODE GENERATOR
AST_Node*
new_ast_node ( int node, int node_type, AST_Node* left, AST_Node* right, AST_Node* center ) {
 
	AST_Node* ast_node = new(AST_Node);
	
	ast_node -> node = node;
	ast_node ->	nodeType = node_type;

	ast_node -> left = left;
	ast_node -> right = right;
	ast_node -> center = center;
	
	return ast_node;
}



// SPECIFIC NODE GENERATORS

AST_Node*
new_ast_expInteger_node ( int node, int node_type, int value ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.ki = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expFloat_node ( int node, int node_type, float value ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.kf = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expLiteral_node ( int node, int node_type, const char *value ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);

	exp -> nextExpNode = NULL;
	exp -> u.lit = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expFuncCall_node ( int node, int node_type, Call *funcCall ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.functionCall = funcCall;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expVariable_node( int node, int node_type, AST_Node *variableNode ) {

	AST_Node *treeNode;
	Exp *exp = new(Exp);
	
	exp -> nextExpNode = NULL;
	exp -> u.varNode = variableNode;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_variable_node( int node, int node_type, const char *id, AST_Node *exp01, AST_Node *exp02 ) {

	AST_Node *treeNode = NULL;
	Var *var = new(Var);
	
	var -> nextVarNode = NULL;

	treeNode = new_ast_node(node, node_type, exp01, exp02, NULL);
	
	if( id != NULL)
		var -> varName = id;
	

// if(exp1 == NULL && exp2 == NULL) {
// 		var -> varName = id;
// 		treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
// 	}
// 	else if(id == NULL)
// 		treeNode = new_ast_node(node, node_type, exp1, exp2, NULL);
	
	
	treeNode -> nodeStruct.var = var;

	return treeNode;
}

Call*
new_funcCall(const char* id, AST_Node *expListNode) {

	Call *funcCall = new(Call);
	
	funcCall -> funcName = id;
	funcCall -> expressionNode = expListNode;

	return funcCall;
}


// OTHER FUNCTIONS

AST_Node*
connect_exp_list(AST_Node *father, AST_Node *son) {

	father -> nodeStruct.exp -> nextExpNode = son;
	return father;
}

static void* myMalloc(size_t size) {
	
	void* obj = (void *)malloc(size);
	
	if(obj == NULL) { 	
		printf("MEMORY SHORTAGE ERROR\n"); 
		exit(0); 	
	}	
	return obj;
}



// PRINT FUNCTIONS



int main (void) {
	
	
}

