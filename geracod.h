#ifndef GERACOD_H
#define GERACOD_H

#include "ast.h"
#define MAX_TAM 500

static int currentLabel = 0;
static int countTemp = 0;
char fileName[50];
extern const char *buffer[MAX_TAM];

char lastTemp[50];

void geraCodigo ( AST_Node *root );
extern void initialBuffer ();
extern void setFilename ( char *name );

#endif