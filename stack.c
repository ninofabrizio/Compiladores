#include "stack.h"

Stack* 
stack_create (void) {
	
	Stack* s = (Stack*)malloc(sizeof(Stack));
	s -> prim = NULL;
	return s;
		
}

void
stack_push (Stack *p, symbol_table *t) {
	
	Node* n = (Node*)malloc(sizeof(Node));
	n -> info = t;
	n -> prox = p -> prim;
	p -> prim = n;
	
}

symbol_table* 
stack_pop (Stack *p) {
	
	Node* n;
    symbol_table *t;
	
	if (stack_empty(p)) {		
		printf("Empty Stack.\n");
		exit(1);	
	}
	
	n = p -> prim;
	t = n -> info;	
	p -> prim = n -> prox;
	free(n);
	
	return t;
		
}

int 
stack_empty (Stack* p) {
	
	return (p -> prim == NULL);
}

void
stack_free (Stack* p) {
	
	Node* q = p -> prim;
	
	while (q!=NULL) {		
	
		Node* t = q -> prox;
		free(q);
		q = t;		
	
	}
	
	free(p);

}
