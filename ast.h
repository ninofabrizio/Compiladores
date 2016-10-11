#if !defined AST_H
#define AST_H

/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>*/

Typedef enum nodeEnum nodeEnum;
Typedef enum structEnum structEnum;
Typedef struct AST_NodeType AST_NodeType;

// For the type of the node
enum nodeEnum {

	EXPR,
	VARI,
	DEFI,
	STAT,
	TYPE
};

// For all the types of structs
enum structEnum {

	EXPR_OR,
	EXPR_AND,
	EXPR_COMP,
	EXPR_ADD_MIN,
	EXPR_MUL_DIV,
	EXPR_UNA,
	EXPR_VAR,
	EXPR_PART,
	EXPR_FUNC_CALL,
	EXPR_NEW
};

struct AST_NodeType {

	// Flags
	nodeEnum typeNode;
	structEnum typeStruct;

	Typedef union nodeStruct {

		//STRUCTS HERE

	} nodeStruct;
};


// DOWN HERE EACH INDIVIDUAL STRUCT

#endif