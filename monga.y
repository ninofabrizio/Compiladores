%{

	#include "monga.h"
	#include "ast.h"
	
	extern void yyerror(char *);
	extern int yyparse(void);
	int yylex(void);
	int currentLine;

%}

%union {
    
    int i;
    double f;
    const char *s;

    /*struct id {

    	const char* name;
    	int currentLine;
    } identifier;*/

    AST_NodeType *node;
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

%type <node> definition definitions varDefinition funcDefinition type baseType nameList nameSequence parameters parameter parametersSequence block varDefSequence commandSequence expression command ifElseCommand variable expressionOptional funcCalling expressionPrim expressionOr expressionAnd expressionComp expressionAddMin expressionMulDiv expressionUna numeral literal expList expressionSequence

%start program // nós 'recursivos': definitions nameSequence parametersSequence varDefSequence commandSequence command ifElseCommand expressionSequence

%%

program: definitions ;	{ $$ = $1; }


definitions: definition definitions	{  }
			|	{  } ;


definition: varDefinition	{  }
			| funcDefinition	{  } ;


varDefinition: type nameList ';'	{  } ;


nameList: TK_ID nameSequence	{  } ;


nameSequence: ',' TK_ID nameSequence	{  }
				|	{  } ;


type : baseType	{  }
		| type '[' ']'	{  } ;


baseType : TK_WORD_INT	{  }
			| TK_WORD_CHAR	{  }
			| TK_WORD_FLOAT	{  } ;


funcDefinition: TK_WORD_VOID TK_ID '(' parameters ')' block	{  }
				| type  TK_ID '(' parameters ')' block	{  } ;


parameters: parameter parametersSequence	{  }
			|	{  } ;


parametersSequence: ',' parameter parametersSequence	{  }
					|	{  } ;


parameter:	type TK_ID 	{  } ;


block:	'{' varDefSequence commandSequence '}'	{  } ;


varDefSequence: varDefinition varDefSequence	{  }
				|	{  } ;


commandSequence: command commandSequence	{  }
				|	{  } ;


command: TK_WORD_IF '(' expression ')' command 	{  }
		 | TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE command 	{  }
		 | TK_WORD_WHILE '(' expression ')' command 	{  }
		 | variable '=' expression ';'	{  }
		 | TK_WORD_RETURN expressionOptional ';'	{  }
		 | funcCalling ';'	{  }
		 | block	{  } ;


ifElseCommand: TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE ifElseCommand	{  }
			   | TK_WORD_WHILE '(' expression ')' ifElseCommand	{  }
			   | variable '=' expression ';'	{  }
			   | TK_WORD_RETURN expressionOptional ';'	{  }
			   | funcCalling ';'	{  }
			   | block	{  } ;  


expressionOptional: expression 	{  }
					|	{  } ;


variable: TK_ID 	{  }
			| expressionPrim '[' expression ']'	{  } ;


expression : expressionOr	{ $$ = $1; } ; // certo?


expressionOr :	expressionOr TK_OR expressionAnd	{  -+- }
				| expressionAnd	{  } ;


expressionAnd :	expressionAnd TK_AND expressionComp	{  }
				| expressionComp	{  } ;


expressionComp :	expressionComp TK_EQUAL expressionAddMin	{  }
					| expressionComp TK_LESS_OR_EQ expressionAddMin	{  }
					| expressionComp TK_GREATER_OR_EQ expressionAddMin	{  }
					| expressionComp '<' expressionAddMin	{  }
					| expressionComp '>' expressionAddMin	{  }
					| expressionComp TK_NOT_EQ expressionAddMin	{  }
					| expressionAddMin	{  } ;


expressionAddMin :	expressionAddMin '+' expressionMulDiv	{  }
					| expressionAddMin '-' expressionMulDiv	{  }
					| expressionMulDiv	{  } ;


expressionMulDiv :	expressionMulDiv '*' expressionUna	{  }
					| expressionMulDiv '/' expressionUna	{  }
					| expressionUna	{  } ;


expressionUna :	'!' expressionPrim	{  }
				| '-' expressionPrim	{  }
				| expressionPrim	{  } ;


expressionPrim:	numeral	{  }
				| literal	{  }
				| variable	{  }
				| '(' expression ')'	{  }
				| funcCalling	{  }
				| TK_WORD_NEW type '[' expression ']'	{  } ;


funcCalling: TK_ID '(' expList ')'	{  } ;


expList: expression expressionSequence	{  }
		|	{  } ;


expressionSequence: ',' expression expressionSequence	{  }
					|	{  } ;


numeral: TK_INTEGER	{  }
			| TK_HEXA	{  }
			| TK_FLOAT 	{  } 
			| TK_CHAR 	{  } ;


literal: TK_LIT_STRING	{  } ;

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