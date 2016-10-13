#if !defined AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

typedef enum nodeEnum nodeEnum;
typedef enum nodeTypeEnum nodeTypeEnum;
typedef struct Call Call;
typedef struct Type Type;
typedef struct Var Var;
typedef struct Exp Exp;
typedef struct Param Param;
typedef struct Defvar DefVar;
typedef struct Block Block;
typedef struct Def Def;
typedef struct Stat Stat;
typedef struct IfElse IfElse;
typedef struct AST_Node AST_Node;
typedef struct AST_PrimaryExp_Node AST_PrimaryExp_Node;


AST_Node* new_ast_node ( int node, int node_type, AST_Node* left, AST_Node* right, AST_Node* center );
AST_PrimaryExp_Node* new_ast_primary_node ( int node, int node_type, Exp *primary );



enum nodeEnum {

	EXPR,
	VAR,
	DEF,
	STAT,
	TYPE
};

enum nodeTypeEnum {
	
	TChar,
	TInt,
	TFloat,
	Tarray,
	
	VarID,
	VarINDEXED,
	
	STAT_WHILE,
	
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

struct Call {
	
	const char *funcName;
	int openPar;
	int closePar;
	Exp *expression;
	
};

struct Type {
	
	int tokenType;
};

struct Var {
	
	struct Var *proxElement; 
	
	union {	
		const char *varName;		
		struct { struct Exp *exp01, *exp02; } indexed;					
	} u;
};

struct Exp {
	
	struct Exp *proxExp;
	
	union {
		Var *var;	
		int ki;	
		float kf;
		Call *functionCall;
		struct { struct Exp *exp01, *exp02; } bin;	
		struct { int symbol; Exp *exp00; } una;
		struct { int openPar; int closePar; Exp *exp00; } par;
		struct { int newWord; Type* dataType; int openBracket; int closeBracket; Exp *exp00; } new;	
	} u;
};

struct Param {
	
	struct Param *proxParam;
	Type *dataType;
	const char *paramName;
};

struct Defvar { 
	
	Type *dataType; 
	Var **nameList;
}; 

struct Block { 

	int openCurveBracket;
	int closeCurveBracket;

	DefVar **varList;
	Stat **statList;
};

struct Def {
	
	union {
		
		DefVar *var;
		struct { const char *funcName; int openPar; int closePar; Param *param; 
		     	 union { int voidType; Type *dataType; } ret;
			 	 int tagReturnType; // to void return 0; to other types return 1.
				 Block *block;
		} func;		
	} u;
};

struct Stat {
	
	union {
		Call *callFunc;
		Block *block;
		struct { Var *var; int assignType; Exp *exp00; } assign;
		struct { int returnType; Exp *exp00; } retCommand;
		struct { int whileType; Exp *exp00; struct Stat *commandList; } whileLoop;
		struct { int ifType; int openPar; int closePar; Exp *exp00; struct Stat *block; } ifCondition;
		IfElse *ifElseCondition;
	} u;
};

struct IfElse {
		
	struct IfElse *proxIfElse;
	struct { int ifType; int openPar; int closePar; Exp *exp00; struct Stat *commandBlock; } ifCondition;
	struct { int elseType; struct Stat *commandBlock; } elseCondition;
};



// node no-terminal
struct AST_Node 
{
	nodeEnum node;
	nodeTypeEnum nodeType;
	AST_Node *left;
	AST_Node *right;
	AST_Node *center; // only to ifElse condition
	
};

// terminal node
struct AST_PrimaryExp_Node { 
	
	nodeEnum node;
	nodeTypeEnum nodeType;
	Exp* primaryExp;	
};


#endif