%{

#include "monga.h"

/* Variável para posição de linha corrente no arquivo de leitura */
int currentLine = 1;

/* Variável para guardar linha onde começou comentário */
int openComment;

/* Função para fazer copia de string */
char * strDup (char *s);

/* Função para notificação de erro */
void errorMsg (char *msg);

/* Função para notificação de erro de comentário não fechado */
void commentError();

%}

Exp				[Ee]
Digit			[0-9]
Letter			[A-Za-z]

StringLit			([^\"\n]|\\'|\\n|\\t|\\\\|\\\"|\\0)
Character		    ([^'\n]|\\'|\\n|\\t|\\\\|\\\"|\\0)

%x Comment	

%%

[ \t]+						{ ; }
\n 							{ currentLine++; }


"char"					 			{ return TK_WORD_CHAR; }
"float"								{ return TK_WORD_FLOAT; }
"int"								{ return TK_WORD_INT; }
"if" 								{ return TK_WORD_IF; }
"else" 								{ return TK_WORD_ELSE; }
"while"								{ return TK_WORD_WHILE; }
"new"								{ return TK_WORD_NEW; }
"return"							{ return TK_WORD_RETURN; }
"void"								{ return TK_WORD_VOID; }


({Letter}|_)({Letter}|{Digit}|_)*	        { return TK_ID; }


{Digit}+[.]?						        { return TK_INTEGER;}  // Criar o TK_NUMBER. subrotina p/o tipo de variável.
({Digit}*[.]?{Digit}+)({Exp}[+-]?{Digit})?  { return TK_DOUBLE;}  //  Criar TK_DOUBLE. subrotina p/o tipo de variável.
{Digit}+{Letter}+ 					        { errorMsg("Digit before letter"); return -1; }


'\\'								{ errorMsg("Invalid character"); return -1; } // específico demais?
'{Character}?'						{ yyvar.s = strDup(yytext); return TK_CHAR; }
'{Character}+'						{ errorMsg("Invalid character"); return -1; }


\"\\\"								{ errorMsg("Invalid string"); return -1; } // específico demais?
\"{StringLit}*\"					{ yyvar.s = strDup(yytext); return TK_LIT_STRING; }


"/*"								{ openComment = currentLine; BEGIN (Comment); }
<Comment>[^*\n]*        			{ ; }
<Comment>"*"+[^*/\n]*   			{ ; }
<Comment>\n 						{ currentLine++; }
<Comment>"*"+"/"        			{ printf("%d\n", currentLine); BEGIN(INITIAL); } // nao esquecer de tirar o printf
<Comment><<EOF>>					{ commentError(); return -1; }


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


.									{ errorMsg("Invalid token entry"); return -1; }


%%

/* Função externa para guardar arquivo de leitura */
void setInput (FILE *file) {

	yyin = file;
}

void errorMsg (char *msg) {

	printf("\nError type '%s': token '%s' in line %d\n", msg, yytext, currentLine);
}

void commentError() {

	printf("\nError type 'Comment not closed': from line %d to line %d\n", openComment, currentLine);
}

char * strDup (char *s) {

    char *d = (char *) malloc (sizeof(char)*(strlen(s) + 1));  
    if (d == NULL)
    	return NULL;         

    strcpy (d,s);                        
    return d;                            
}