#if !defined MONGA_H
#define MONGA_H

#include <stdio.h>
#include <stdlib.h>

typedef enum Token {
 
 	NOT_TOKEN			= 666,
    TK_DOUBLE 			= 1,
    TK_ID				= 2,
    TK_PLUS				= 3,
    TK_MULT				= 4,
    TK_ASG				= 5,
    TK_SEMIC			= 6,
	TK_EQUAL			= 7,
	TK_NOT_EQ			= 8,
	TK_LESS 			= 9,
	TK_LESS_OR_EQ		= 10,
	TK_GREATER 			= 11,
	TK_GREATER_OR_EQ	= 12,
	TK_MINUS			= 13,
	TK_DIV				= 14,
	TK_LEFT_PARENT		= 15,
	TK_RIGHT_PARENT		= 16,
	TK_LEFT_BRAC 		= 17,
	TK_RIGHT_BRAC 		= 18,
	TK_LEFT_CURV_BRAC	= 19,
	TK_RIGHT_CURV_BRAC	= 20,
	TK_OR 				= 21,
	TK_AND				= 22,
	TK_COM				= 23,
	TK_NOT 				= 24,
	TK_WORD_CHAR		= 25,
	TK_WORD_FLOAT		= 26,
	TK_WORD_INT			= 27,
	TK_WORD_IF			= 28,
	TK_WORD_ELSE		= 29,
	TK_WORD_WHILE		= 30,
	TK_WORD_NEW			= 31,
	TK_WORD_RETURN		= 32,
	TK_WORD_VOID		= 33,
	TK_CHAR 			= 34,
	TK_LIT_STRING		= 35,
	TK_INTEGER			= 36
    
} token;

typedef union Types_var {
    
    int i;
    double f;
    const char *s;
    
} types_var;

extern types_var yyvar;
extern int yylex(void);
extern void setInput(FILE *file);

extern types_var seminfo;

#endif