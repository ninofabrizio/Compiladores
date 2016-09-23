%{

	#include "monga.h"

	extern void yyerror(char *);
	int yylex(void);

	extern int currentLine;
%}

%union {
    
    int i;
    double f;
    const char *s;
}

%token	TK_FLOAT
%token	TK_NOT
%token	TK_PLUS
%token	TK_MULT
%token	TK_ASG
%token	TK_SEMIC
%token	TK_COM
%token	TK_RIGHT_CURV_BRAC
%token	TK_LESS
%token	TK_LEFT_CURV_BRAC
%token	TK_GREATER
%token	TK_RIGHT_BRAC
%token	TK_MINUS
%token	TK_DIV
%token	TK_LEFT_PARENT
%token	TK_RIGHT_PARENT
%token	TK_LEFT_BRAC
%token	TK_CHAR
%token	TK_LESS_OR_EQ
%token	TK_NOT_EQ
%token	TK_OR
%token	TK_AND
%token	TK_EQUAL
%token	TK_ID
%token	TK_WORD_CHAR
%token	TK_WORD_FLOAT
%token	TK_WORD_INT
%token	TK_WORD_IF
%token	TK_WORD_ELSE
%token	TK_WORD_WHILE
%token	TK_WORD_NEW
%token	TK_WORD_RETURN
%token	TK_WORD_VOID
%token	TK_LIT_STRING
%token	TK_GREATER_OR_EQ
%token	TK_INTEGER
%token	TK_HEXA

%start program

%%

program :	definition // aqui vai um de 0 ou mais (ver sintaxe no site)
			;


definition :	varDefinition
				|	funcDefinition
				;


varDefinition :	type nameList TK_SEMIC
				;


nameList :	TK_ID TK_COM TK_ID // aqui vai um de 0 ou mais
			;


type :	baseType
		| type TK_LEFT_BRAC TK_RIGHT_BRAC
		;


baseType :	TK_WORD_INT
			| TK_WORD_CHAR
			| TK_WORD_FLOAT
			;


funcDefinition :	retType TK_ID TK_LEFT_PARENT parameters TK_RIGHT_PARENT block
					;


retType :	type
			| TK_WORD_VOID
			;


parameters :	parameter TK_LEFT_CURV_BRAC TK_COM parameter TK_RIGHT_CURV_BRAC // aqui vai um de 0 ou mais dentro de um opcional (ver sintaxe no site)
				;


parameter :	type TK_ID
			;


block :	TK_LEFT_CURV_BRAC varDefinition command TK_RIGHT_CURV_BRAC // aqui vão 2 de 0 ou mais
		;


command :	TK_WORD_IF TK_LEFT_PARENT expression TK_RIGHT_PARENT command TK_WORD_ELSE command // aqui vai um de 0 ou mais
			| TK_WORD_WHILE TK_LEFT_PARENT expression TK_RIGHT_PARENT command
			| variable TK_ASG expression TK_SEMIC
			| TK_WORD_RETURN expression TK_SEMIC // aqui vai um opcional
			| funcCalling TK_SEMIC
			| block
			;


variable :	TK_ID
			| expression TK_LEFT_BRAC expression TK_RIGHT_BRAC
			;


expression :	numeral
				| literal
				| variable
				| TK_LEFT_PARENT expression TK_RIGHT_PARENT
				| funcCalling
				| TK_WORD_NEW type TK_LEFT_BRAC expression TK_RIGHT_BRAC
				| TK_MINUS expression
				| expression TK_PLUS expression
				| expression TK_MINUS expression
				| expression TK_MULT expression
				| expression TK_DIV expression
				| expression TK_EQUAL expression
				| expression TK_LESS_OR_EQ expression
				| expression TK_GREATER_OR_EQ expression
				| expression TK_LESS expression
				| expression TK_GREATER expression
				| TK_NOT expression
				| expression TK_AND expression
				| expression TK_OR expression
				;


funcCalling:	TK_ID TK_LEFT_PARENT expList TK_RIGHT_PARENT
				;


expList :	expression TK_COM expression // aqui vai um opcional dentro de um de 0 ou mais
			;


numeral :	TK_INTEGER
			| TK_HEXA
			| TK_FLOAT
			| TK_CHAR
			;


literal :	TK_LIT_STRING // falta alguem?
			;

%%

void yyerror(char *s) {

 	fprintf(stderr, "\n%s in line %d\n", s, currentLine);
}