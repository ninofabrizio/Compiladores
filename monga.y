%{

	#include "monga.h"

	extern void yyerror(char *);
	int yylex(void);
	int currentLine;
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

program: definitions ;


definitions: definition definitions | ;


definition: varDefinition | funcDefinition ;


varDefinition: type nameList TK_SEMIC ;


nameList: TK_ID nameSequence ;


nameSequence: TK_COM TK_ID nameSequence | ;


type : baseType | type TK_LEFT_BRAC TK_RIGHT_BRAC ;


baseType : TK_WORD_INT | TK_WORD_CHAR | TK_WORD_FLOAT ;


funcDefinition: TK_WORD_VOID TK_ID TK_LEFT_PARENT parameters TK_RIGHT_PARENT block
				| type  TK_ID TK_LEFT_PARENT parameters TK_RIGHT_PARENT block ;


parameters: parameter parametersSequence | ;


parametersSequence: TK_COM parameter parametersSequence | ;


parameter:	type TK_ID ;


block:	TK_LEFT_CURV_BRAC varDefSequence commandSequence TK_RIGHT_CURV_BRAC ;


varDefSequence: varDefinition varDefSequence | ;


commandSequence: command commandSequence | ;


command: TK_WORD_IF TK_LEFT_PARENT expression TK_RIGHT_PARENT command
		 | TK_WORD_IF TK_LEFT_PARENT expression TK_RIGHT_PARENT ifElseCommand TK_WORD_ELSE command
		 | TK_WORD_WHILE TK_LEFT_PARENT expression TK_RIGHT_PARENT command
		 | variable TK_ASG expression TK_SEMIC
		 | TK_WORD_RETURN expressionOptional TK_SEMIC
		 | funcCalling TK_SEMIC
		 | block ;


ifElseCommand: TK_WORD_IF TK_LEFT_PARENT expression TK_RIGHT_PARENT ifElseCommand TK_WORD_ELSE ifElseCommand
			   | TK_WORD_WHILE TK_LEFT_PARENT expression TK_RIGHT_PARENT ifElseCommand
			   | variable TK_ASG expression TK_SEMIC
			   | TK_WORD_RETURN expressionOptional TK_SEMIC
			   | funcCalling TK_SEMIC
			   | block ;  


expressionOptional: expression | ;


variable: TK_ID | expressionPrim TK_LEFT_BRAC expression TK_RIGHT_BRAC ;


expression : expressionOr ;


expressionOr :	expressionOr TK_OR expressionAnd
				| expressionAnd ;


expressionAnd :	expressionAnd TK_AND expressionComp
				| expressionComp ;


expressionComp :	expressionComp TK_EQUAL expressionAddMin
					| expressionComp TK_LESS_OR_EQ expressionAddMin
					| expressionComp TK_GREATER_OR_EQ expressionAddMin
					| expressionComp TK_LESS expressionAddMin
					| expressionComp TK_GREATER expressionAddMin
					| expressionComp TK_NOT_EQ expressionAddMin
					| expressionAddMin ;


expressionAddMin :	expressionAddMin TK_PLUS expressionMulDiv
					| expressionAddMin TK_MINUS expressionMulDiv
					| expressionMulDiv ;


expressionMulDiv :	expressionMulDiv TK_MULT expressionUna
					| expressionMulDiv TK_DIV expressionUna
					| expressionUna ;


expressionUna :	TK_NOT expressionPrim
				| TK_MINUS expressionPrim
				| TK_LEFT_PARENT type TK_RIGHT_PARENT expressionPrim // Array pode ser tambem typecast?
				| expressionPrim ;


expressionPrim:	numeral
				| literal
				| variable
				| TK_LEFT_PARENT expression TK_RIGHT_PARENT
				| funcCalling
				| TK_WORD_NEW type TK_LEFT_BRAC expression TK_RIGHT_BRAC ;


funcCalling: TK_ID TK_LEFT_PARENT expList TK_RIGHT_PARENT ;


expList: expression expressionSequence | ;


expressionSequence: TK_COM expression expressionSequence | ;


numeral: TK_INTEGER | TK_HEXA | TK_FLOAT | TK_CHAR ;


literal: TK_LIT_STRING ;

%%

void yyerror(char *s) {

 	fprintf(stderr, "\n%s in line %d\n", s, currentLine);
}