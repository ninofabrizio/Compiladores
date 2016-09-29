%{

	#include "monga.h"	
	
	extern void yyerror(char *);
	extern int yyparse(void);
	int yylex(void);
	int currentLine;

%}

%union {
    
    int i;
    double f;
    const char *s;
}

%token	TK_FLOAT
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

program: definitions ;


definitions: definition definitions | ;


definition: varDefinition | funcDefinition ;


varDefinition: type nameList ';' ;


nameList: TK_ID nameSequence ;


nameSequence: ',' TK_ID nameSequence | ;


type : baseType | type '[' ']' ;


baseType : TK_WORD_INT | TK_WORD_CHAR | TK_WORD_FLOAT ;


funcDefinition: TK_WORD_VOID TK_ID '(' parameters ')' block
				| type  TK_ID '(' parameters ')' block ;


parameters: parameter parametersSequence | ;


parametersSequence: ',' parameter parametersSequence | ;


parameter:	type TK_ID ;


block:	'{' varDefSequence commandSequence '}' ;


varDefSequence: varDefinition varDefSequence | ;


commandSequence: command commandSequence | ;


command: TK_WORD_IF '(' expression ')' command
		 | TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE command
		 | TK_WORD_WHILE '(' expression ')' command
		 | variable '=' expression ';'
		 | TK_WORD_RETURN expressionOptional ';'
		 | funcCalling ';'
		 | block ;


ifElseCommand: TK_WORD_IF '(' expression ')' ifElseCommand TK_WORD_ELSE ifElseCommand
			   | TK_WORD_WHILE '(' expression ')' ifElseCommand
			   | variable '=' expression ';'
			   | TK_WORD_RETURN expressionOptional ';'
			   | funcCalling ';'
			   | block ;  


expressionOptional: expression | ;


variable: TK_ID | expressionPrim '[' expression ']' ;


expression : expressionOr ;


expressionOr :	expressionOr TK_OR expressionAnd
				| expressionAnd ;


expressionAnd :	expressionAnd TK_AND expressionComp
				| expressionComp ;


expressionComp :	expressionComp TK_EQUAL expressionAddMin
					| expressionComp TK_LESS_OR_EQ expressionAddMin
					| expressionComp TK_GREATER_OR_EQ expressionAddMin
					| expressionComp '<' expressionAddMin
					| expressionComp '>' expressionAddMin
					| expressionComp TK_NOT_EQ expressionAddMin
					| expressionAddMin ;


expressionAddMin :	expressionAddMin '+' expressionMulDiv
					| expressionAddMin '-' expressionMulDiv
					| expressionMulDiv ;


expressionMulDiv :	expressionMulDiv '*' expressionUna
					| expressionMulDiv '/' expressionUna
					| expressionUna ;


expressionUna :	'!' expressionPrim
				| '-' expressionPrim
				| '(' type ')' expressionPrim // Array pode ser tambem typecast?
				| expressionPrim ;


expressionPrim:	numeral
				| literal
				| variable
				| '(' expression ')'
				| funcCalling
				| TK_WORD_NEW type '[' expression ']' ;


funcCalling: TK_ID '(' expList ')' ;


expList: expression expressionSequence | ;


expressionSequence: ',' expression expressionSequence | ;


numeral: TK_INTEGER | TK_HEXA | TK_FLOAT | TK_CHAR ;


literal: TK_LIT_STRING ;

%%

void yyerror(char *s) {

 	fprintf(stderr, "\n%s in line %d\n", s, currentLine);
}