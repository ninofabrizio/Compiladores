#include "symtab.h"


extern symbol_table* 
symbol_table_create (void) {
	
	symbol_table* sym = (symbol_table*)malloc(sizeof(symbol_table));
	sym -> prim = NULL;
	return sym;
}

extern void 
symbol_table_destroy (symbol_table* table) {
	
	id_entry* entry = table -> prim;
	
	while(entry != NULL) {
				
		id_entry* entry_aux = entry -> prox;
		free(entry);
		entry = entry_aux;	
	}
	
	free(table);		
}


extern id_entry* 
symbol_table_find_entry (symbol_table *table, const char *name) {
	
	id_entry* entry;
	
	for (entry = table -> prim; entry != NULL; entry = entry -> prox) 
		if ( (strcmp(entry->name_id, name) ) == 0) 		
			return entry;			
			
	return NULL;

}


extern symbol_table* 
insert (symbol_table *table, const char *name, void* nodeRef, boolean *present) {
	
	
	symbol_table* table_aux = (symbol_table*)malloc(sizeof(symbol_table));
	id_entry* entry;
	
	entry = symbol_table_find_entry ( table, name);
	
	if (entry != NULL) {
	
		*present = true;
		return table;
	
	}
	
	entry = (id_entry*)malloc(sizeof(id_entry));
	entry -> name_id = name;
	entry -> nodeRef = nodeRef;
	entry -> prox = table -> prim;
	
	table_aux -> prim = entry;
	*present = false;
	
	return table_aux;
		
}
