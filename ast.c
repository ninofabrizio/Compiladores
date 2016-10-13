#include "ast.h"

#define new(T) ((T*)malloc(sizeof(T)))

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

AST_PrimaryExp_Node*
new_ast_primary_node ( int node, int node_type, Exp *primary ) {
	
	AST_PrimaryExp_Node* ast_primary_node = new(AST_PrimaryExp_Node);
	ast_primary_node -> node = node;
	ast_primary_node ->	nodeType = node_type;
	ast_primary_node -> primaryExp = primary;

	return ast_primary_node;
}


// AST_Node*
// new_ast_while_node ( AST_Node *condition, AST_Node *while_branch ) {
// 	return new_ast_node ( STAT, STAT_WHILE, condition, while_branch, NULL );
// }


int main (void) {
	
	return 0;
	
}















