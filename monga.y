%{

	#include "monga.h"
	#include "ast.h"
	
	extern void yyerror(char *);
	extern int yyparse(void);
	int yylex(void);

	int currentLine;
	AST_Node *AST_Root;

%}


%union {
    
    int i;
    double f;
    const char *s;

    /*struct id {

    	const char* name;
    	int currentLine;
    } identifier;*/

    AST_Node *node;
    Call *call;
}


%token <f>	TK_FLOAT
%token <i>	TK_CHAR
%token <i>	TK_LESS_OR_EQ
%token <i>	TK_NOT_EQ
%token <i>	TK_OR
%token <i>	TK_AND
%token <i>	TK_EQUAL
%token <s>	TK_ID
%token <i>	TK_WORD_CHAR
%token <i>	TK_WORD_FLOAT
%token <i>	TK_WORD_INT
%token <i>	TK_WORD_IF
%token <i>	TK_WORD_ELSE
%token <i>	TK_WORD_WHILE
%token <i>	TK_WORD_NEW
%token <i>	TK_WORD_RETURN
%token <i>	TK_WORD_VOID
%token <s>	TK_LIT_STRING
%token <i>	TK_GREATER_OR_EQ
%token <i>	TK_INTEGER
%token <i>	TK_HEXA

<<<<<<< HEAD

%type <node> definition definitions varDefinition funcDefinition type baseType nameList nameSequence parameters parameter parametersSequence block varDefSequence commandSequence expression command ifElseCommand variable expressionOptional funcCalling expressionPrim expressionOr expressionAnd expressionComp expressionAddMin expressionMulDiv expressionUna numeral literal expList expressionSequence
=======
%type <node> program definition definitions varDefinition funcDefinition type baseType nameList nameSequence parameters parameter parametersSequence block varDefSequence commandSequence expression command ifElseCommand variable expressionOptional expressionPrim expressionOr expressionAnd expressionComp expressionAddMin expressionMulDiv expressionUna numeral literal expList expressionSequence
%type <call> funcCalling
>>>>>>> 96950048985e484cb0fde77ef38ff8c2793473fe

%start program 


%%

<<<<<<< HEAD
program: definitions { $$ = new_ast_node(ROOT, ROOT, $1, null, null); AST_Root = $$; };




definitions: definition definitions	{ $$ = new_ast_node(DEF, DEF, $1, $2, null); }
			
			| { $$ = NULL; } ;




definition: varDefinition { $$ = $1; }
			
			| funcDefinition { $$ = $1; } ;




varDefinition: type nameList ';' { $$ = new_ast_node(DEF, DEF_VAR, $1, $2, null); } ;



nameList: TK_ID nameSequence { $$ = $1; } ;




nameSequence: ',' TK_ID nameSequence { $$ = $1; }
				
			| { $$ = NULL; } ;




type: baseType	{ $$ = $1; }
		
	| type '[' ']'	{ $$ = $1; } ;




baseType: TK_WORD_INT	{ $$ = $1; }
			
		| TK_WORD_CHAR	{ $$ = $1; }
			
		| TK_WORD_FLOAT	{ $$ = $1; } ;


=======
program: definitions { $$ = $1; AST_Root = $$; } ; // This might not be the right way to instantiate our root...
>>>>>>> 96950048985e484cb0fde77ef38ff8c2793473fe


funcDefinition: TK_WORD_VOID TK_ID '(' parameters ')' block	{ $$ = new_ast_node(DEF, DEF_FUNC, $3, $5, null); }
				
				| type  TK_ID '(' parameters ')' block	{ $$ = new_ast_node(DEF, DEF_FUNC, $1, $4, $6); } ;




parameters: parameter parametersSequence	{ $$ = new_ast_node(PARAM, PARAM, $1, $2, null); }
			
			| { $$ = NULL; } ;




parametersSequence: ',' parameter parametersSequence	{ $$ = new_ast_node(PARAM, PARAM, $2, $3, null); }
					
					| { $$ = NULL; } ;




parameter:	type TK_ID 	{ $$ = $1; } ;




block:	'{' varDefSequence commandSequence '}'	{ $$ = new_ast_node(BLOCK, BLOCK, $1, $2, null); }
		
		| { $$ = NULL; } ;




varDefSequence: varDefinition varDefSequence	{ $$ = new_ast_node(VAR, VAR_DEF, $1, $2, null); }
				
				| { $$ = NULL; } ;




commandSequence: command commandSequence	{ $$ = new_ast_node(STAT, STAT, $1, $2, null); }
				
				| { $$ = NULL; } ;



command: TK_WORD_IF '(' expression ')' command 	{ $$ = new_ast_node(STAT, STAT_IF, $3, $5, null); }
						 
		| TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE command { $$ = new_ast_node(STAT, STAT_IFELSE, $3, $5, $7); }
						 
		| TK_WORD_WHILE '(' expression ')' command { $$ = new_ast_node(STAT, STAT_WHILE, $3, $5, null); }
						
		| variable '=' expression ';'	{ $$ = new_ast_node(STAT, STAT_ASSIGN, $1, $3, null); }
						 
		| TK_WORD_RETURN expressionOptional ';' { $$ = new_ast_node(STAT, STAT_RETURN, $2, null, null); }
						 
		| funcCalling ';'	{  $$ = $1; }
		
		| block	{ $$ = $1;  } ;




ifElseCommand: TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE ifElseCommand	{ $$ = new_ast_node(STAT, STAT_IFELSE, $3, $5, $7); }
			   
			   | TK_WORD_WHILE '(' expression ')' ifElseCommand	{ $$ = new_ast_node(STAT, STAT_WHILE, $3, $5, null); }
			   
			   | variable '=' expression ';'	{ $$ = new_ast_node(STAT, STAT_ASSIGN, $1, $3, null); }
			   
			   | TK_WORD_RETURN expressionOptional ';'	{ $$ = new_ast_node(STAT, STAT_RETURN, $2, null, null); }
			   
			   | funcCalling ';'	{ $$ = $1; }
			   
			   | block	{ $$ = $1; } ;  




expressionOptional: expression 	{ $$ = $1; }
					| { $$ = NULL; } ;




variable:	TK_ID 	{ $$ = new_ast_variable_node(VAR, VAR_UNIQUE, $1, NULL, NULL); }
			
			| expressionPrim '[' expression ']'	{ $$ = new_ast_variable_node(VAR, VAR_INDEXED, NULL, $1, $3); } ;




expression: expressionOr	{ $$ = $1; } ;




expressionOr:	expressionOr TK_OR expressionAnd	{ $$ = new_ast_node(EXPR, EXPR_OR, $1, $3, NULL); }
				
				| expressionAnd	{ $$ = $1; } ;




expressionAnd:	expressionAnd TK_AND expressionComp	{ $$ = new_ast_node(EXPR, EXPR_AND, $1, $3, NULL); }
				
				| expressionComp	{ $$ = $1; } ;




expressionComp:	expressionComp TK_EQUAL expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_EQUAL, $1, $3, NULL); }
					
					| expressionComp TK_LESS_OR_EQ expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_LEEQ, $1, $3, NULL); }
					
					| expressionComp TK_GREATER_OR_EQ expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_GREQ, $1, $3, NULL); }
					
					| expressionComp '<' expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_LESS, $1, $3, NULL); }
					
					| expressionComp '>' expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_GREATER, $1, $3, NULL); }
					
					| expressionComp TK_NOT_EQ expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_NOEQ, $1, $3, NULL); }
					
					| expressionAddMin	{ $$ = $1; } ;




expressionAddMin:	expressionAddMin '+' expressionMulDiv	{ $$ = new_ast_node(EXPR, EXPR_ADD, $1, $3, NULL); }
					
					| expressionAddMin '-' expressionMulDiv	{ $$ = new_ast_node(EXPR, EXPR_MIN, $1, $3, NULL); }
					
					| expressionMulDiv	{ $$ = $1; } ;




expressionMulDiv:	expressionMulDiv '*' expressionUna	{ $$ = new_ast_node(EXPR, EXPR_MUL, $1, $3, NULL); }
					
					| expressionMulDiv '/' expressionUna	{ $$ = new_ast_node(EXPR, EXPR_DIV, $1, $3, NULL); }
					
					| expressionUna	{ $$ = $1; } ;




expressionUna:	'!' expressionPrim	{ $$ = new_ast_node(EXPR, EXPR_NOT, $2, NULL, NULL); }
				
				| '-' expressionPrim	{ $$ = new_ast_node(EXPR, EXPR_NEG, $2, NULL, NULL); }
				
				| expressionPrim	{ $$ = $1; } ;




expressionPrim:	numeral	{ $$ = $1; }
				
				| literal	{ $$ = $1; }
				
				| variable	{ $$ = new_ast_expVariable_node(EXPR, EXPR_VAR, $1); }
				
				| '(' expression ')'	{ $$ = $2; }
				
				| funcCalling	{ $$ = new_ast_expFuncCall_node(EXPR, EXPR_FUNC_CALL, $1); }
				
				| TK_WORD_NEW type '[' expression ']'	{ $$ = new_ast_node(EXPR, EXPR_NEW, $2, $4, NULL); } ;




funcCalling: TK_ID '(' expList ')'	{ $$ = new_funcCall($1, $3); } ;




expList: expression expressionSequence	{ $$ = connect_exp_list($1, $2); }
		
		| { $$ = NULL; } ;




expressionSequence: ',' expression expressionSequence	{ $$ = connect_exp_list($2, $3); }
					
					| { $$ = NULL; } ;




numeral: TK_INTEGER	{ $$ = new_ast_expInteger_node (EXPR, EXPR_INT, $1); }
			
			| TK_HEXA	{ $$ = new_ast_expInteger_node (EXPR, EXPR_HEXA, $1); }
			
			| TK_FLOAT 	{ $$ = new_ast_expFloat_node (EXPR, EXPR_FLOAT, $1); } 
			
			| TK_CHAR 	{ $$ = new_ast_expInteger_node (EXPR, EXPR_CHAR, $1); } ;




literal: TK_LIT_STRING	{ $$ = new_ast_expLiteral_node (EXPR, EXPR_LIT, $1); } ;




%%

void yyerror(char *s) {

 	fprintf(stderr, "\n%s in line %d with symbol ", s, currentLine);

 	if((yychar >= 32) && (yychar <= 126))
		fprintf(stderr, "'%c'\n", yychar);
	else if((yychar >= 260) && (yychar <= 276))
		fprintf(stderr, "'%s'\n", yylval.s);
	else if(yychar == 258)
		fprintf(stderr, "'%f'\n", yylval.f);
	else if(yychar == 259)
		fprintf(stderr, "'%c'\n", yylval.i);
	else if(yychar == 277)
		fprintf(stderr, "'%d'\n", yylval.i);
	else if(yychar == 278)
		fprintf(stderr, "'%x'\n", yylval.i);
	else
		fprintf(stderr, "number %d\n", yychar);
}