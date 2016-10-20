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

    AST_Node *node;
    Call *call;
	
	Param *param;
	Block *block;

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

%type <node> program definition definitions varDefinition funcDefinition type baseType nameList nameSequence varDefSequence commandSequence expression command ifElseCommand variable expressionOptional expressionPrim expressionOr expressionAnd expressionComp expressionAddMin expressionMulDiv expressionUna numeral literal expList expressionSequence
%type <call> funcCalling
%type <param> parameters parameter parametersSequence
%type <block> block


%start program

%%

<<<<<<< HEAD
program: definitions { $$ = $1; AST_Root = $$; } ; 


definitions: definition definitions	{ $$ = new_ast_node(DEF, DEF, $1, $2, NULL, currentLine); } 
=======
program: definitions { AST_Root = $1; } ; // Acho desnecessário criar um nó pra raíz, na teoria ele já existe (aí fica um nó de nó neste caso)


definitions: definition definitions	{ $$ = connect_definitions($1, $2); } // 1: Definição de Definição? 2: Acho que o certo aqui é incluir uma lista na struct de Definição (vide lista de expressões no final das regras)
>>>>>>> a9d9e16d90c81d0a603326be15a38691e14bf467
			|	{ $$ = NULL; } ;


definition: varDefinition	{ $$ = $1; }
			| funcDefinition	{ $$ = $1; } ;


varDefinition: type nameList ';'	{ $$ = new_ast_defVariable_node(DEF, DEF_VAR, $1, $2); } ; // Acho que o certo aqui é usar uma estrutura em $2. A struct Var é candidata, já que ela possui o ponteiro para sequência de variáveis ou rever a struct de DefVar. Não esquecer do açúcar sintático dito na aula (int a, b, c; == int a; int b; int c;).


nameList: TK_ID nameSequence	{ $$ = new_ast_variable_node(VAR, VAR_UNIQUE, $1, NULL, NULL, $2, currentLine); } ;


nameSequence: ',' TK_ID nameSequence	{ $$ = new_ast_variable_node(VAR, VAR_UNIQUE, $2, NULL, NULL, $3, currentLine); } // Lista de variáveis aqui
				|	{ $$ = NULL; } ;


type: baseType	{ $$ = $1; } // Nenhuma estrutura gerada aqui?
		| type '[' ']'	{ $$ = isArrayType($1); } ;


baseType: TK_WORD_INT	{ $$ = new_ast_type_node(TYPE, TYPE_INT, "int", currentLine); } // Nenhuma estrutura gerada aqui? Lembrando que com o que mudei em monga.l, esses tokens possuem o currentLine
			| TK_WORD_CHAR	{ $$ = new_ast_type_node(TYPE, TYPE_CHAR, "char", currentLine); }
			| TK_WORD_FLOAT	{ $$ = new_ast_type_node(TYPE, TYPE_FLOAT, "float", currentLine); } ;









funcDefinition: TK_WORD_VOID TK_ID '(' parameters ')' block	{ $$ = new_func_def( "void", $2, $4, $6, NULL, currentLine ); }

				| type  TK_ID '(' parameters ')' block { $$ = new_func_def( NULL, $2, $4, $6, $1, currentLine ); } 
;


parameters: parameter parametersSequence { $$ = Param* connect_param_list( $1, $2 ); } 
			|	{ $$ = NULL; } 
;


parametersSequence: ',' parameter parametersSequence { $$ = Param* connect_param_list( $1, $2 ); } 
					|	{ $$ = NULL; } 
;

parameter:	type TK_ID 	{ $$ = new_param( $1, $2, NULL); } 
; 

block:	'{' varDefSequence commandSequence '}'	{ $$ = connect_node($2, $3); } 
; 

varDefSequence: varDefinition varDefSequence	{ $$ = connect_node($1, $2); }
				|	{ $$ = NULL; } 
;

commandSequence: command commandSequence	{ $$ = connect_node($2, $3); }
				|	{ $$ = NULL; } 
;



command: TK_WORD_IF '(' expression ')' command 	{ $$ = new_ast_node(STAT, STAT_IF, $3, $5, NULL, $1); }
		 
		 | TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE command 	{ $$ = new_ast_node(STAT, STAT_IFELSE, $3, $5, $7, $6); }
		 
		 | TK_WORD_WHILE '(' expression ')' command 	{ $$ = new_ast_node(STAT, STAT_WHILE, $3, $5, NULL, $1); }
		 
		 | variable '=' expression ';'	{ $$ = new_ast_node(STAT, STAT_ASSIGN, $1, $3, NULL, currentLine); }
		 
		 | TK_WORD_RETURN expressionOptional ';'	{ $$ = new_ast_node(STAT, STAT_RETURN, $2, NULL, NULL, $1); }
		 
		 | funcCalling ';'	{ $$ = new_command_func_calling( $1, currentLine ); } 
		 
		 | block	{ $$ = $1; } 
;



ifElseCommand: TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE ifElseCommand	{ $$ = new_ast_node(STAT, STAT_IFELSE, $3, $5, $7, $6); }
			   
			   | TK_WORD_WHILE '(' expression ')' ifElseCommand	{ $$ = new_ast_node(STAT, STAT_WHILE, $3, $5, NULL, $1); }
			   
			   | variable '=' expression ';'	{ $$ = new_ast_node(STAT, STAT_ASSIGN, $1, $3, NULL, currentLine); }
			   
			   | TK_WORD_RETURN expressionOptional ';'	{ $$ = new_ast_node(STAT, STAT_RETURN, $2, NULL, NULL, $1); }
			   
			   | funcCalling ';'	{ $$ = new_command_func_calling( $1, currentLine ); } 
			   
			   | block	{ $$ = $1; } 
;


expressionOptional: expression 	{ $$ = $1; }
					|	{ $$ = NULL; } ;


variable:	TK_ID 	{ $$ = new_ast_variable_node(VAR, VAR_UNIQUE, $1, NULL, NULL, NULL, currentLine); }
			| expressionPrim '[' expression ']'	{ $$ = new_ast_variable_node(VAR, VAR_INDEXED, NULL, $1, $3, NULL, currentLine); } ;


expression: expressionOr	{ $$ = $1; } ;


expressionOr:	expressionOr TK_OR expressionAnd	{ $$ = new_ast_node(EXPR, EXPR_OR, $1, $3, NULL, $2); }
				| expressionAnd	{ $$ = $1; } ;


expressionAnd:	expressionAnd TK_AND expressionComp	{ $$ = new_ast_node(EXPR, EXPR_AND, $1, $3, NULL, $2); }
				| expressionComp	{ $$ = $1; } ;


expressionComp:	expressionComp TK_EQUAL expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_EQUAL, $1, $3, NULL, $2); }
					| expressionComp TK_LESS_OR_EQ expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_LEEQ, $1, $3, NULL, $2); }
					| expressionComp TK_GREATER_OR_EQ expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_GREQ, $1, $3, NULL, $2); }
					| expressionComp '<' expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_LESS, $1, $3, NULL, currentLine); }
					| expressionComp '>' expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_GREATER, $1, $3, NULL, currentLine); }
					| expressionComp TK_NOT_EQ expressionAddMin	{ $$ = new_ast_node(EXPR, EXPR_NOEQ, $1, $3, NULL, $2); }
					| expressionAddMin	{ $$ = $1; } ;


expressionAddMin:	expressionAddMin '+' expressionMulDiv	{ $$ = new_ast_node(EXPR, EXPR_ADD, $1, $3, NULL, currentLine); }
					| expressionAddMin '-' expressionMulDiv	{ $$ = new_ast_node(EXPR, EXPR_MIN, $1, $3, NULL, currentLine); }
					| expressionMulDiv	{ $$ = $1; } ;


expressionMulDiv:	expressionMulDiv '*' expressionUna	{ $$ = new_ast_node(EXPR, EXPR_MUL, $1, $3, NULL, currentLine); }
					| expressionMulDiv '/' expressionUna	{ $$ = new_ast_node(EXPR, EXPR_DIV, $1, $3, NULL, currentLine); }
					| expressionUna	{ $$ = $1; } ;


expressionUna:	'!' expressionPrim	{ $$ = new_ast_node(EXPR, EXPR_NOT, $2, NULL, NULL, currentLine); }
				| '-' expressionPrim	{ $$ = new_ast_node(EXPR, EXPR_NEG, $2, NULL, NULL, currentLine); }
				| expressionPrim	{ $$ = $1; } ;


expressionPrim:	numeral	{ $$ = $1; }
				| literal	{ $$ = $1; }
				| variable	{ $$ = new_ast_expVariable_node(EXPR, EXPR_VAR, $1); }
				| '(' expression ')'	{ $$ = $2; }
				| funcCalling	{ $$ = new_ast_expFuncCall_node(EXPR, EXPR_FUNC_CALL, $1, currentLine); }
				| TK_WORD_NEW type '[' expression ']'	{ $$ = new_ast_node(EXPR, EXPR_NEW, $2, $4, NULL, $1); } ;


funcCalling: TK_ID '(' expList ')'	{ $$ = new_funcCall($1, $3); } ;


expList: expression expressionSequence	{ $$ = connect_exp_list($1, $2); }
		|	{ $$ = NULL; } ;


expressionSequence: ',' expression expressionSequence	{ $$ = connect_exp_list($2, $3); }
					|	{ $$ = NULL; } ;


numeral: TK_INTEGER	{ $$ = new_ast_expInteger_node (EXPR, EXPR_INT, $1, currentLine); }
			| TK_HEXA	{ $$ = new_ast_expInteger_node (EXPR, EXPR_HEXA, $1, currentLine); }
			| TK_FLOAT 	{ $$ = new_ast_expFloat_node (EXPR, EXPR_FLOAT, $1, currentLine); } 
			| TK_CHAR 	{ $$ = new_ast_expInteger_node (EXPR, EXPR_CHAR, $1, currentLine); } ;


literal: TK_LIT_STRING	{ $$ = new_ast_expLiteral_node (EXPR, EXPR_LIT, $1, currentLine); } ;

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