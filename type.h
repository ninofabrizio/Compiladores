#if !defined TYPE_H
#define TYPE_H

#include "ast.h"

typedef struct AST_Node AST_Node;

typedef enum typeEnum typeEnum;
typedef enum valueEnum valueEnum;

typedef struct Typing Typing;

extern void type_tree( AST_Node *a );

enum typeEnum {
	
	INTEGER,
	FLOAT,
	VOID,
	ARRAY,
	STRING_TYPE
};

enum valueEnum {
	
	NONE,
	INT_VALUE,
	FLOAT_VALUE,
	STRING
};


struct Typing { 

		Typing *nextTyping; // array case

		typeEnum typeKind;
		//OU Type *type;

		valueEnum type;

		union {
			int intValue;
			float floatValue;
			const char *string;
		} typeValue;
};

#endif