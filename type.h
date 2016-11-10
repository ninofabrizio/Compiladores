#if !defined TYPE_H
#define TYPE_H

#include "ast.h"

typedef enum typeEnum typeEnum;
typedef enum valueEnum valueEnum;

typedef struct AST_Node AST_Node;

typedef struct Typing Typing;

extern void type_tree( AST_Node *a );

enum typeEnum {
	
	INTEGER,
	FLOAT,
	VOID,
	ARRAY
};

enum valueEnum {
	
	INT_VALUE,
	FLOAT_VALUE
};


struct Typing { 

		typeEnum typeKind;
		//OU Type *type;

		valueEnum tagValue;

		union {
			int intValue;
			float floatValue;
		} typeValue;
};

#endif