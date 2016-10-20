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
new_ast_variable_node( int node, int node_type, const char *id, AST_Node *exp01, AST_Node *exp02, int line ) {

	AST_Node *treeNode;
	Var *var = new(Var);
	
	var -> nextVarNode = NULL;

	treeNode = new_ast_node(node, node_type, exp01, exp02, NULL, line);
	
	if(id != NULL)
		var -> varName = id;	
	
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





AST_Node*
new_func_def( const char* returnType, const char *funcName, Param *param, Block *block, AST_Node *node, int line ) {
	
	AST_Node *funcNode = new_ast_node(DEF, DEF_FUNC, node, NULL, NULL, line);
	
	(funcNode -> nodeStruct.def) -> u.func.block = block;
	(funcNode -> nodeStruct.def) -> u.func.param = param;
	(funcNode -> nodeStruct.def)-> u.func.funcName = funcName;
	(funcNode -> nodeStruct.def)-> u.func.tagReturnType = 0;
	(funcNode -> nodeStruct.def)-> u.func.ret.voidType = returnType;
	
	return funcNode;
}

Param*
new_param( AST_Node *type, const char *paramName, Param *nextParam) {
	
	Param *paramNode = new(Param);
	
	paramNode -> paramName = paramName; 
	paramNode -> dataTypeNode = type; 
	paramNode -> proxParam = nextParam;
	
	return paramNode;
		
}


Param*
connect_param_list( Param *father, Param *son ) {
	
	father -> proxParam = son;	
	return father;
		
}

AST_Node*
connect_node(AST_Node *varDef, AST_Node *commandSeq) {
	
	varDef -> right = commandSeq;
	return varDef;	
}


AST_Node*
new_command_func_calling( Call *func, int line ) {
	
	AST_Node *funcNode = new_ast_node(STAT, STAT_FUNC_CALL, NULL, NULL, NULL, line);
	
	(funcNode -> nodeStruct.stat) -> u.callFunc = func; 
	
	return funcNode;	
}




// PRINT AND MEMORY RELEASE FUNCTIONS



int main (void) {
	
	
}

