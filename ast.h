#if !defined AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

typedef enum nodeEnum nodeEnum;
typedef enum nodeTypeEnum nodeTypeEnum;

typedef struct AST_Node AST_Node;

typedef struct Call Call;
typedef struct Type Type;
typedef struct Var Var;
typedef struct Exp Exp;
typedef struct Param Param;
typedef struct DefVar DefVar;
typedef struct Def Def;
typedef struct Stat Stat;

AST_Node* new_ast_node ( int node, int node_type, AST_Node* left, AST_Node* right, AST_Node* center, int line );
AST_Node* new_ast_expInteger_node ( int node, int node_type, int value, int line );
AST_Node* new_ast_expFloat_node ( int node, int node_type, float value, int line );
AST_Node* new_ast_expLiteral_node ( int node, int node_type, const char *value, int line );
AST_Node* new_ast_expFuncCall_node ( int node, int node_type, Call *funcCall, int line );
AST_Node* new_ast_expVariable_node( int node, int node_type, AST_Node *variableNode );
AST_Node* new_ast_variable_node( int node, int node_type, const char *id, AST_Node *exp1, AST_Node *exp2, AST_Node *nextVarNode, int line );
AST_Node* new_ast_type_node( int node, int node_type, const char *baseType, int line );
AST_Node* new_ast_defVariable_node( int node, int node_type, AST_Node* typeNode, AST_Node* varListNode );
AST_Node* new_func_def( const char* returnType, const char *funcName, Param *param, AST_Node *block, AST_Node *node, int line );
AST_Node* new_command_func_calling( Call *func, int line );
Call* new_funcCall( const char* id, AST_Node *expListNode );
Param* new_param( AST_Node *type, const char *paramName, Param *nextParam );
AST_Node* connect_exp_list( AST_Node *father, AST_Node *son );
Param* connect_param_list( Param *father, Param *son );
AST_Node* isArrayType( AST_Node *typeNode );
AST_Node*connect_node_left(AST_Node *varDef, AST_Node *commandSeq);
AST_Node*connect_node_right(AST_Node *varDef, AST_Node *commandSeq);
AST_Node* new_stat_if( int i, int j, AST_Node* n1, AST_Node* n2, AST_Node* n3, int line);
AST_Node* new_stat_while( int i, int j, AST_Node* n1, AST_Node* n2, int line);
AST_Node* new_stat_assign( int i, int j, AST_Node* n1, AST_Node* n2, int line);
AST_Node* new_stat_ret( int i, int j, AST_Node* n1, int line);
extern void print_tree(AST_Node *a);


enum nodeEnum {

	EXPR,
	VAR,
	DEF,
	STAT,
	TYPE
};

enum nodeTypeEnum {
	
	TYPE_CHAR,
	TYPE_INT,
	TYPE_FLOAT,
	
	VAR_UNIQUE,
	VAR_INDEXED,
	
	DEF_VAR,
	DEF_FUNC,
	
	STAT_WHILE,
	STAT_IF,
	STAT_IFELSE,
	STAT_ASSIGN,	
	STAT_RETURN,
	STAT_FUNC_CALL,
		
	EXPR_OR,
	EXPR_AND,
	EXPR_EQUAL,
	EXPR_LEEQ,
	EXPR_GREQ,
	EXPR_GREATER,
	EXPR_LESS,
	EXPR_NOEQ,
	EXPR_ADD,
	EXPR_MIN,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_NOT,
	EXPR_NEG,
	EXPR_VAR,
	EXPR_FUNC_CALL,
	EXPR_NEW,
	EXPR_INT,
	EXPR_HEXA,
	EXPR_CHAR,
	EXPR_FLOAT,
	EXPR_LIT
};

// Tree node

struct AST_Node 
{
	nodeEnum node;
	nodeTypeEnum nodeType;
	int line;

	AST_Node *right;
	AST_Node *left;
	AST_Node *center; // only for ifElse condition

	union {

		Def  *def;
		Stat *stat;
		Type *type;
		Var  *var;
		Exp  *exp;
		
	} nodeStruct;
};

// Structs

struct Call {
	
	const char *funcName;

	AST_Node *expressionNode;
	
};

struct Type {
	
	const char *baseType;

	int arraySequence; // counter == 0 if not array | >= 1 if array (we can have array of array of array...)
};

struct Var {
	
	AST_Node *nextVarNode; 
	
	const char *varName;
};

struct Exp {
	
	AST_Node *nextExpNode;
	
	union {
		AST_Node *varNode;
		const char *lit;
		int ki;
		float kf;
		Call *functionCall;
	} u;
};

struct Param {
	
	Param *proxParam;
	DefVar *var;
	
};


struct DefVar { 

	AST_Node *dataTypeNode; 
	AST_Node *varListNode; 
};

struct Def {
	
	union {
		DefVar *defVar;
		struct { const char *funcName; Param *param; 
		     	 union { const char *voidType; AST_Node *dataTypeNode; } ret;
			 	 int tagReturnType; // for void return 0; for other types return 1.
				 AST_Node *block;
		} func;		
	} u;
};

struct Stat {
	
	union {
		Call *callFunc;
		AST_Node *block;
		struct { AST_Node *varNode; AST_Node *exp00Node; } assign;
		struct { int returnType; AST_Node *exp00Node; } retCommand;
		struct { int whileType; AST_Node *exp00Node; AST_Node *commandListNode; } whileLoop;
		struct { AST_Node *exp00Node; AST_Node *block; AST_Node *elseNo; } ifCondition;
	} u;
};

#endif