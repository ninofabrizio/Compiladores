#if !defined AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

typedef enum nodeEnum nodeEnum;
typedef enum structEnum structEnum;
typedef struct AST_NodeType AST_NodeType;

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
	
	TChar,
	TInt,
	TFloat,
	Tarray,
	
	VarID,
	VarINDEXED,
	
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


typedef struct Call {
	
	const char* id;
	int openPar;
	int closePar;
	Exp *e;
	
} Call;


typedef struct Type {
	
	int tokenType;
		
} Type;


typedef struct Var {
	
	struct Var *prox; // p/Array
	
	union {
		
		const char *id;		
		struct { struct exp *exp1, *exp2; } indexed;			
		
	} u;

} Var;


typedef struct Exp {
	
	Exp *proxExp;
	
	union {
	
		struct { Exp *e1, *e2 } bin;	
		struct { int symbol; Exp *e } una;
		struct { int open; int close; Exp *e } par;
		struct { int new; Type* type; int open; int close; Exp *exp } new;
		
		Var *var;	
		int ki;
		float kf;
	
		Call *functionCall;

	} u;
		
} Exp;


typedef struct Param {
	
	Type *type;
	const char* id;
	struct Param *listParam;	

} Param;


typedef struct { 
	struct Def *definitions;  
	Stat *statements 
} Block;


typedef struct IfElse {
	
	union {
	
		Call *callFunction;
		Block *block;

		struct { int ifType; int open; int close; Exp *exp; struct IfElse *command; IfElse *ifElse } ifControl;

		struct { int whileType; int open; int close; Exp *exp; struct IfElse *command; } whileControl;

		struct { Var *var; int assignType; Exp *e } Assign;

		struct {  int returnType; Exp *e } retCommand;	
	
	} u;
		
} IfElse;


typedef struct Stat {
	
	union {
	
		Call *callFunction;
		Block *block;
	
		struct { int ifType; int open; int close; Exp *exp; struct Stat *command; IfElse *ifElse } ifControl;
	
		struct { int whileType; int open; int close; Exp *exp; struct Stat *command; } whileControl;
	
		struct { Var *var; int assignType; Exp *e } Assign;
	
		struct {  int returnType; Exp *e } retCommand;	
	
	} u;
	
	
} Stat;



typedef struct Def {
	

	union {

		struct { Type *type; const char **listID; int semicolon } var;
	
		struct { const char *id; int open; int close; Param *param; 
		     	union{ int wordVoid; Type *type} ret;
			 	int tag; // 0..void; 1..type
		 } func;
	
		 Block *block;
		
	} u;


} Def;



struct AST_NodeType {

	// Flags
	nodeEnum typeNode;
	structEnum typeStruct;

	AST_NodeType *right;
	AST_NodeType *left;

	Typedef union nodeStruct {

		Def  *def;
		Stat *stat;
		Type *type;
		Var  *var;
		Exp  *exp;
		
	} nodeStruct;

};


#endif