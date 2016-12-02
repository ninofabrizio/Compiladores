#ifndef STACK_H
#define STACK_H

#include "symtab.h"

typedef struct stack Stack;
typedef struct node Node;

Stack* stack_create (void);
void stack_push (Stack* p, symbol_table *t);
symbol_table* stack_pop (Stack* p);
int stack_empty (Stack* p);
void stack_free (Stack* p);

struct stack {
	Node* prim;
};

struct node {	
	symbol_table *info;
	struct node *prox;
};

#endif