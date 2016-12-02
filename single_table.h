#ifndef SINGLE_TABLE_H
#define SINGLE_TABLE_H

#include "stack.h"

extern Stack* single_table_create (void);
extern void single_table_destroy (Stack *single_table);
extern id_entry* single_table_find (Stack *single_table, const char *elemFound);
extern id_entry* single_table_find_current_scope (Stack *single_table, const char *elemFound);
extern void single_table_insert_current_scope (Stack *single_table, const char *name, void* nodeRef, boolean *present);
extern symbol_table* single_table_pop_scope (Stack *single_table);
extern void single_table_push_scope (Stack *single_table, symbol_table *newTable);

#endif