#if !defined SYMTAB_H
#define SYMTAB_H

#include<stdlib.h>
#include <string.h>
#include<stdio.h>

typedef enum boolean boolean;
typedef struct symtab symbol_table;
typedef struct id_entry id_entry;

extern symbol_table* symbol_table_create (void);
extern void symbol_table_destroy (symbol_table* table);
extern id_entry* symbol_table_find_entry (symbol_table *table, const char *name);
extern symbol_table* insert (symbol_table *table, const char *type, const char *name, boolean isArray, boolean *present);

enum boolean {
	false,
	true
};

struct symtab {
	
	id_entry* prim;
		
};

struct id_entry {
	
	const char *type;
	boolean isArray;
	const char* name_id;
	struct id_entry* prox;
	
};

#endif