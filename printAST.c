#include "printAST.h"

static void print_def(AST_Node *a, int tabIndex);
static void print_block(AST_Node *a, int tabIndex);
static void print_call (Call *a, int tabIndex);
static void print_stat (AST_Node *a, int tabIndex);
static void print_type(AST_Node *a, int tabIndex);
static void print_var(AST_Node *a, int tabIndex);

void
printTyping ( Typing *typing ) {

	if(typing != NULL) {

		if(typing -> typeKind == ARRAY) {
			printf("ARRAY ");
			printTyping(typing -> nextTyping);
		}
		else if(typing -> typeKind == INTEGER)
			printf("INTEGER\n");
		else if(typing -> typeKind == CHARACTER)
			printf("CHARACTER\n");
		else if(typing -> typeKind == FLOAT)
			printf("FLOAT\n");
		else if(typing -> typeKind == VOID)
			printf("VOID\n");
		else if(typing -> typeKind == STRING_TYPE)
			printf("STRING_TYPE\n");

		free(typing);
	}
}


static void
print_type ( AST_Node *a, int tabIndex ) {
	
	int i, count;
	
	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("TYPE: ");
		printf("%s", a -> nodeStruct.type -> baseType);	
				
		for(i = 0; i < a -> nodeStruct.type -> arraySequence ; i++) 
			printf("[]");
		printf("\n");

		free(a -> nodeStruct.type);		
		free(a);
	}	
}


static void
print_exp (AST_Node *a, int tabIndex) {
	
	int count;

	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		if ( a -> nodeType == EXPR_VAR ) {
			
			printf("Line [%d] EXPR_VAR {\n", a -> line);	
			print_var(a -> nodeStruct.exp -> u.varNode, tabIndex+1);	
			free(a -> nodeStruct.exp -> u.varNode);

		} else if ( a -> nodeType == EXPR_FUNC_CALL ) {
			
			printf("Line [%d] EXPR_FUNC_CALL {\n", a -> line);
			print_call(a -> nodeStruct.exp -> u.functionCall, tabIndex+1);

		}  else if ( a -> nodeType == EXPR_NEW ) {
			
			printf("Line [%d] EXPR_NEW {\n", a -> line);
			print_type(a->left, tabIndex+1);
			print_exp(a->right, tabIndex+1);
			a->left = NULL;
			a->right = NULL;
			
		}  else if ( a -> nodeType == EXPR_INT ) {
			
			printf("Line [%d] EXPR_INT {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %d\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_HEXA ) {
			
			printf("Line [%d] EXPR_HEXA {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %#x\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_CHAR ) {
			
			printf("Line [%d] EXPR_CHAR {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %c\n", a -> nodeStruct.exp -> u.ki);

		}  else if ( a -> nodeType == EXPR_FLOAT ) {
			
			printf("Line [%d] EXPR_FLOAT {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: %f\n", a -> nodeStruct.exp -> u.kf);
			
		}  else if ( a -> nodeType == EXPR_LIT ) {
			
			printf("Line [%d] EXPR_LIT {\n", a -> line);

			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VALUE: \"%s\"", a -> nodeStruct.exp -> u.lit);
			
		} else if ( a -> nodeType == EXPR_OR ) {
			
			printf("Line [%d] EXPR_OR {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_AND ) {
			
			printf("Line [%d] EXPR_AND {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_EQUAL ) {
			
			printf("Line [%d] EXPR_EQUAL {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_LEEQ ) {
			
			printf("Line [%d] EXPR_LEEQ {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_GREQ ) {
			
			printf("Line [%d] EXPR_GREQ {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_GREATER ) {
			
			printf("Line [%d] EXPR_GREATER {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_LESS ) {
			
			printf("Line [%d] EXPR_LESS {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_NOEQ ) {
			
			printf("Line [%d] EXPR_NOEQ {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_ADD ) {
			
			printf("Line [%d] EXPR_ADD {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_MIN ) {
			
			printf("Line [%d] EXPR_MIN {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_MUL ) {
			
			printf("Line [%d] EXPR_MUL {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_DIV ) {
			
			printf("Line [%d] EXPR_DIV {\n", a -> line);
			
		} else if ( a -> nodeType == EXPR_NOT ) {
			
			printf("Line [%d] EXPR_NOT {\n", a -> line);
			
		} else {
			
			printf("Line [%d] EXPR_NEG {\n", a -> line);
		}

		print_exp(a->left, tabIndex+1);
		print_exp(a->right, tabIndex+1);		

		print_exp(a -> nodeStruct.exp -> nextExpNode, tabIndex+1);	
		
		if(a -> nodeStruct.exp -> typing != NULL) {
				
			//printf("\n");
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("TYPING: ");
			printTyping(a -> nodeStruct.exp -> typing);
		}

		free(a -> nodeStruct.exp);

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		free(a);
	}	
}


static void
print_var( AST_Node *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
				
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		if ( a -> nodeType == VAR_UNIQUE ) {
			
			printf("VAR_UNIQUE {\n");

			if(a -> nodeStruct.var -> typing != NULL) {
				
				//printf("\n");
				for(count = 0; count < tabIndex+1; count++)
					printf("\t");

				printf("TYPING: ");
				printTyping(a -> nodeStruct.var -> typing);
			}

			//printf("\n");
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("VAR NAME: %s\n", a -> nodeStruct.var -> varName);

		}
		else {		
			
			printf("VAR_INDEXED {\n");
		
			if(a -> nodeStruct.var -> typing != NULL) {
				
				//printf("\n");
				for(count = 0; count < tabIndex+1; count++)
					printf("\t");

				printf("TYPING: ");
				printTyping(a -> nodeStruct.var -> typing);
			}

			print_exp(a->left, tabIndex+1);
			print_exp(a->right, tabIndex+1);
		
			a->left = NULL;
			a->right = NULL;
		
		}
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_var(a -> nodeStruct.var -> nextVarNode, tabIndex);
		
		print_tree(a->left, tabIndex);
		print_tree(a->right, tabIndex);

		free(a -> nodeStruct.var);
	}
}


static void
print_params ( Param *p, int tabIndex ) {
	
	int count;

	if( p != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("PARAM {\n");
		print_type(p -> var -> dataTypeNode, tabIndex+1);
		print_var(p -> var -> varListNode, tabIndex+1);
		
		//if ( p -> var -> varListNode -> nodeType == VAR_INDEXED)
		//	printf("%s\n", p -> var -> varListNode -> nodeStruct.var -> varName);
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_params(p -> proxParam, tabIndex);			
	
		free(p -> var -> varListNode);
		free(p -> var);
		free(p);
	}
}


static void
print_stat ( AST_Node *a, int tabIndex ) {
	
	int count;
	
	if( a != NULL ) {

		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		
		if( a -> nodeType == STAT_WHILE ) {
			
			printf("Line [%d] STAT_WHILE {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.whileLoop.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.whileLoop.commandListNode, tabIndex+1);
				
		} else if ( a -> nodeType == STAT_IF ) {
		
			printf("Line [%d] STAT_IF {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block, tabIndex+1);
		
		} else if( a -> nodeType == STAT_IFELSE ) {
			
			printf("Line [%d] STAT_IFELSE {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.ifCondition.exp00Node, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.block, tabIndex+1);
			print_stat(a -> nodeStruct.stat -> u.ifCondition.elseNo, tabIndex+1);	
		
		} else if( a -> nodeType == STAT_ASSIGN ) {
			
			printf("Line [%d] STAT_ASSIGN {\n", a -> line);
			print_var(a -> nodeStruct.stat -> u.assign.varNode, tabIndex+1);
			print_exp(a -> nodeStruct.stat -> u.assign.exp00Node, tabIndex+1);
			free(a -> nodeStruct.stat -> u.assign.varNode);
			
		} else if( a -> nodeType == STAT_RETURN ) {
			
			printf("Line [%d] STAT_RETURN {\n", a -> line);
			print_exp(a -> nodeStruct.stat -> u.returnExp00Node, tabIndex+1);
		
		} else {
		
			printf("Line [%d] STAT_FUNC {\n", a -> line);
			print_call(a -> nodeStruct.stat -> u.callFunc, tabIndex+1);	
		}
	
		if(a -> nodeStruct.stat -> typing != NULL) {
				
			//printf("\n");
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");

			printf("TYPING: ");
			printTyping(a -> nodeStruct.stat -> typing);
		}

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_tree(a->right, tabIndex);
		print_tree(a->left, tabIndex);
		
		free(a);
	}	
}


static void
print_call ( Call *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("FUNC_CALL {\n");

		for(count = 0; count < tabIndex+1; count++)
			printf("\t");
		
		printf("FUNC NAME: %s\n", a -> funcName);

		for(count = 0; count < tabIndex+1; count++)
			printf("\t");
		printf("PARAMETERS {\n");

		print_exp(a -> expressionNode, tabIndex+2);

		for(count = 0; count < tabIndex+1; count++)
			printf("\t");
		printf("}\n");

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");
	}

	free(a);
}


static void
print_block( AST_Node *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
		
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");

		printf("BLOCK {\n");
		
		if ( a -> nodeType == DEF_VAR ) {
			
			print_def(a, tabIndex+1);
			//printf("\n");
			print_stat(a->right, tabIndex+1);

		} else	
			print_stat(a, tabIndex+1);

		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");
	}			
}


static void
print_def ( AST_Node *a, int tabIndex ) {
	
	int count;

	if( a != NULL ) {
	
		//printf("\n");
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		
		if( a -> nodeType == DEF_VAR ) {

			printf("Line [%d] DEF_VAR {\n", a -> line);
			
			print_type(a -> nodeStruct.def -> u.defVar -> dataTypeNode, tabIndex+1);
			print_var(a -> nodeStruct.def -> u.defVar -> varListNode, tabIndex+1);

			//if( a -> nodeStruct.def -> u.defVar -> varListNode -> nodeType == VAR_INDEXED )
			//	printf("%s", a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> varName);

			free(a -> nodeStruct.def -> u.defVar -> varListNode);
			free(a -> nodeStruct.def -> u.defVar);

		} else  {
		
			printf("Line [%d] DEF_FUNC {\n", a -> line);

			if(a -> nodeStruct.def -> u.func.tagReturnType == 1)
				print_type(a -> nodeStruct.def -> u.func.ret.dataTypeNode, tabIndex+1);
			else {

				for(count = 0; count < tabIndex+1; count++)
					printf("\t");

				printf("TYPE: void\n");
			}
		
			for(count = 0; count < tabIndex+1; count++)
				printf("\t");
		
			printf("FUNC NAME: %s\n", a -> nodeStruct.def -> u.func.funcName);
			print_params(a -> nodeStruct.def -> u.func.param, tabIndex + 1);
			print_block(a -> nodeStruct.def -> u.func.block, tabIndex + 1);
				
		}
		
		for(count = 0; count < tabIndex; count++)
			printf("\t");
		printf("}\n");

		print_tree(a->left, tabIndex);
		
		if(a -> nodeType == DEF_VAR && a -> nodeStruct.def -> u.defVar -> varListNode -> nodeStruct.var -> isGlobal == true)
			print_tree(a -> right, tabIndex);
	
		free(a -> nodeStruct.def);
		free(a);
	}
}


void
print_tree ( AST_Node *a, int tabIndex ) {
	 
 	if( a != NULL )
 	   switch( a -> node ) {
			case DEF:  print_def(a, tabIndex); break;
			case VAR:  print_var(a, tabIndex); break;
			case EXPR: print_exp(a, tabIndex); break;
			case STAT: print_stat(a, tabIndex); break;	   
			case TYPE: print_type(a, tabIndex); break;
	   	 	default: fprintf(stderr, "AST PRINTING ERROR: UNKNOWN NODE \"%c\"\n", a->nodeType); exit(0);
	 	}
}