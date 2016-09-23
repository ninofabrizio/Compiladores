#if !defined MONGA_H
#define MONGA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void setInput(FILE *file);
extern int yyparse();
extern void yyerror(char *s);
extern int currentLine;

#endif