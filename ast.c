#include "ast.h"

#define new(T) ((T*)malloc(sizeof(T)))

#define myMalloc(P, T) \
			P = new(T); \
			if(P == NULL) { \
				printf("MEMORY SHORTAGE ERROR\n"); \
				exit(0); \
			}


// GENERIC NODE GENERATOR


AST_Node*
new_ast_node ( int node, int node_type, AST_Node* left, AST_Node* right, AST_Node* center ) {
 
	//AST_Node* ast_node = new(AST_Node);
	AST_Node* ast_node;
	myMalloc(ast_node, AST_Node);

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
	//Exp *exp = new(Exp);
	Exp *exp;
	myMalloc(exp, Exp);

	exp -> nextExpNode = NULL;
	exp -> u.ki = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expFloat_node ( int node, int node_type, float value ) {

	AST_Node *treeNode;
	//Exp *exp = new(Exp);
	Exp *exp;
	myMalloc(exp, Exp);

	exp -> nextExpNode = NULL;
	exp -> u.kf = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expLiteral_node ( int node, int node_type, const char *value ) {

	AST_Node *treeNode;
	//Exp *exp = new(Exp);
	Exp *exp;
	myMalloc(exp, Exp);

	exp -> nextExpNode = NULL;
	exp -> u.lit = value;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expFuncCall_node ( int node, int node_type, Call *funcCall ) {

	AST_Node *treeNode;
	//Exp *exp = new(Exp);
	Exp *exp;
	myMalloc(exp, Exp);

	exp -> nextExpNode = NULL;
	exp -> u.functionCall = funcCall;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_expVariable_node( int node, int node_type, AST_Node *variableNode ) {

	AST_Node *treeNode;
	//Exp *exp = new(Exp);
	Exp *exp;
	myMalloc(exp, Exp);

	exp -> nextExpNode = NULL;
	exp -> u.varNode = variableNode;

	treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	treeNode -> nodeStruct.exp = exp;

	return treeNode;
}

AST_Node*
new_ast_variable_node( int node, int node_type, const char *id, AST_Node *exp1, AST_Node *exp2 ) {

	AST_Node *treeNode;
	//Var *var = new(Var);
	Var *var;
	myMalloc(var, Var);

	var -> nextVarNode = NULL;

	if(exp1 == NULL && exp2 == NULL) {
		var -> varName = id;
		treeNode = new_ast_node(node, node_type, NULL, NULL, NULL);
	}
	else if(id == NULL)
		treeNode = new_ast_node(node, node_type, exp1, exp2, NULL);
	
	treeNode -> nodeStruct.var = var;

	return treeNode;
}


// NON-NODE GENERATORS


Call*
new_funcCall(const char* id, AST_Node *expListNode) {

	//Call *funcCall = new(Call);
	Call * funcCall;
	myMalloc(funcCall, Call);

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

// AST_Node*
// new_ast_while_node ( AST_Node *condition, AST_Node *while_branch ) {
// 	return new_ast_node ( STAT, STAT_WHILE, condition, while_branch, NULL );
// }


int main (void) {
	
	return 0;
	
}