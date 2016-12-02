#include "single_table.h"


Stack* 
single_table_create (void) {
	
	return stack_create ();	
}


void 
single_table_destroy (Stack *single_table) {
	
	symbol_table *table;
	
	while (!stack_empty(single_table)) {
		
		table = stack_pop (single_table);
		symbol_table_destroy (table);
			
	}	

	free(single_table);
}


id_entry* 
single_table_find (Stack *single_table, const char *elemFound) {
	
	Node *node;
	id_entry *entry;
	
	for (node = single_table -> prim; node != NULL; node = node -> prox) {
		
		entry = symbol_table_find_entry (node -> info, elemFound);
		
		if (entry != NULL)
			
			return entry;		
	}
	
	return NULL;
}


id_entry* 
single_table_find_current_scope (Stack *single_table, const char *elemFound) {
	
	return symbol_table_find_entry (single_table -> prim -> info, elemFound);
}



void
single_table_insert_current_scope (Stack *single_table, const char *name, void* nodeRef, boolean *present) {
	
	single_table -> prim -> info = insert (single_table -> prim -> info, name, nodeRef, present);
}



symbol_table* 
single_table_pop_scope (Stack *single_table) {
	
	return stack_pop (single_table);
}



void 
single_table_push_scope (Stack *single_table, symbol_table *newTable) {
	
	stack_push (single_table, newTable);
}
