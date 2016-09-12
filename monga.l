%{

#include "monga.h"

%}

Digit			[0-9]
Letter			[A-Za-z]
Character		([^'\n]|\\'|\\n|\\t|\\\\|\\\"|\\0)
LiteralString	([^\"\n]|\\'|\\n|\\t|\\\\|\\\"|\\0)
	
%x Comment	
%%

"=="	 							{ return TK_EQUAL; }
"!="     							{ return TK_NOT_EQ; }
"="      							{ return TK_ASG; }
"<="     							{ return TK_LESS_OR_EQ; }
"<"      							{ return TK_LESS; }
">="     							{ return TK_GREATER_OR_EQ; }
">"      							{ return TK_GREATER; }
"+"      							{ return TK_PLUS; }
"-"      							{ return TK_MINUS; }
"*"      							{ return TK_MULT; }
"/"      							{ return TK_DIV; }
"("      							{ return TK_LEFT_PARENT; }
")"      							{ return TK_RIGHT_PARENT; }
"["		 							{ return TK_LEFT_BRAC; }
"]"		 							{ return TK_RIGHT_BRAC; }
"{"		 							{ return TK_LEFT_CURV_BRAC; }
"}"		 							{ return TK_RIGHT_CURV_BRAC; }
"||"	 							{ return TK_OR;  }
"&&"	 							{ return TK_AND; }
"!"									{ return TK_NOT; }
","		 							{ return TK_COM; }
";"      							{ return TK_SEMIC; }

"char"								{ return TK_WORD_CHAR; }
"float"								{ return TK_WORD_FLOAT; }
"int"								{ return TK_WORD_INT; }
"if" 								{ return TK_WORD_IF; }
"else" 								{ return TK_WORD_ELSE; }
"while"								{ return TK_WORD_WHILE; }
"new"								{ return TK_WORD_NEW; }
"return"							{ return TK_WORD_RETURN; }
"void"								{ return TK_WORD_VOID; }

({Letter}|_)({Letter}|{Digit}|_)*	{ return TK_ID; }

{Digit}+{Letter}+ 					{ return NOT_TOKEN; }
{Digit}+		              		{ return TK_DECIMAL; }

'\\'								{ return NOT_TOKEN; } // específico demais?
'{Character}?'						{ return TK_CHAR; }
'{Character}+'						{ return NOT_TOKEN; }

\"\\\"								{ return NOT_TOKEN; } // específico demais?
\"{LiteralString}*\"				{ return TK_LIT_STRING; }

"/*"								{ printf("hi\n"); BEGIN (Comment); }

<Comment>[^*\n]*        			{ ; }
<Comment>"*"+[^*/\n]*   			{ ; }

<Comment>"*"+"/"        			{ printf("bye\n"); BEGIN(INITIAL); }

[ \t\n]+							{ ; }

.									{ return NOT_TOKEN; }



%%

// Função para guardar arquivo de leitura
void setOutput (FILE *file) {

	yyin = file;
}