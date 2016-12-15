#if !defined TYPE_H
#define TYPE_H

#include "ast.h"

typedef struct AST_Node AST_Node;
typedef struct Typing Typing;
typedef enum typeEnum typeEnum;

extern void type_tree ( AST_Node *a );

enum typeEnum {
	
	INTEGER,
	FLOAT,
	CHARACTER,
	VOID,
	ARRAY,
	STRING_TYPE
};


struct Typing { 

		Typing *nextTyping; // array case

		typeEnum typeKind;
};

#endif