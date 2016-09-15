%{

#include "monga.h"

/* Union para os valores semânticos */
types_var yyvar;

/* Variável para posição de linha corrente no arquivo de leitura */
int currentLine = 1;

/* Variável para guardar linha onde começou comentário */
int openComment;

/* Função para tratar char */
char charTreat(char first, char second);

/* Função para tratar copia de string */
char * strDup (char *s);

/* Função para notificação de erro */
void errorMsg (char *msg);

/* Função para notificação de erro de comentário não fechado */
void commentError();

%}

Exp				[Ee]
Digit			[0-9]
Letter			[A-Za-z]

StringLit			([^\"\n\\]|\\'|\\n|\\t|\\\\|\\\"|\\0)
Character		    ([^'\n]|\\'|\\n|\\t|\\\\|\\\"|\\0)

%x Comment	

%%

[ \t]+						{ ; }
\n 							{ currentLine++; }


"char"					 			{ yyvar.s = yytext; return TK_WORD_CHAR; }
"float"								{ yyvar.s = yytext; return TK_WORD_FLOAT; }
"int"								{ yyvar.s = yytext; return TK_WORD_INT; }
"if" 								{ yyvar.s = yytext; return TK_WORD_IF; }
"else" 								{ yyvar.s = yytext; return TK_WORD_ELSE; }
"while"								{ yyvar.s = yytext; return TK_WORD_WHILE; }
"new"								{ yyvar.s = yytext; return TK_WORD_NEW; }
"return"							{ yyvar.s = yytext; return TK_WORD_RETURN; }
"void"								{ yyvar.s = yytext; return TK_WORD_VOID; }


({Letter}|_)({Letter}|{Digit}|_)*	        { yyvar.s = yytext; return TK_ID; }


{Digit}+[.]?						                    { yyvar.i = strtol(yytext, NULL, 10); return TK_INTEGER;}
({Digit}*[.]{Digit}+)({Exp}[+-]?{Digit}+)?[Ff]?	        { yyvar.f = strtof(yytext, NULL);     return TK_FLOAT; }
{Digit}+[Ff]                                            { yyvar.f = strtof(yytext, NULL);     return TK_FLOAT; }
{Digit}+({Exp}[+-]?{Digit}+)[Ff]?						{ yyvar.f = strtof(yytext, NULL);     return TK_FLOAT; }
[0][Xx]({Digit}+|[A-Fa-f]+)+							{ yyvar.i = strtol(yytext, NULL, 16); return TK_HEXA;  }
({Digit}+{Letter}+)+ 					                { errorMsg("Digit before letter");    return -1; } // CONFERIR ESTA LINHA !!!!!


('\\')|('')							{ errorMsg("Invalid character"); return -1; }
'{Character}?'						{ yyvar.i = charTreat(yytext[1], yytext[2]);
                                      if(yyvar.i == -1) {
                                        errorMsg("Invalid character");
                                        return -1;
                                      }
                                      return TK_CHAR; }
'{Character}+'						{ errorMsg("Invalid character"); return -1; }


\"\\\"								{ errorMsg("Invalid string"); return -1; }
\"{StringLit}*\"					{ yyvar.s = strDup(yytext);
                                      if(yyvar.s == NULL) {
                                        errorMsg("Invalid string");
                                        return -1; 
                                      }
                                      return TK_LIT_STRING; }


"/*"								{ openComment = currentLine; BEGIN (Comment); }
<Comment>[^*\n]*        			{ ; }
<Comment>"*"+[^*/\n]*   			{ ; }
<Comment>\n 						{ currentLine++; }
<Comment>"*"+"/"        			{ BEGIN(INITIAL); }
<Comment><<EOF>>					{ commentError(); return -1; }


"=="	 							{ yyvar.s = yytext; return TK_EQUAL; }
"!="     							{ yyvar.s = yytext; return TK_NOT_EQ; }
"="      							{ yyvar.i = yytext[0]; return TK_ASG; }
"<="     							{ yyvar.s = yytext; return TK_LESS_OR_EQ; }
"<"      							{ yyvar.i = yytext[0]; return TK_LESS; }
">="     							{ yyvar.s = yytext; return TK_GREATER_OR_EQ; }
">"      							{ yyvar.i = yytext[0]; return TK_GREATER; }
"+"      							{ yyvar.i = yytext[0]; return TK_PLUS; }
"-"      							{ yyvar.i = yytext[0]; return TK_MINUS; }
"*"      							{ yyvar.i = yytext[0]; return TK_MULT; }
"/"      							{ yyvar.i = yytext[0]; return TK_DIV; }
"("      							{ yyvar.i = yytext[0]; return TK_LEFT_PARENT; }
")"      							{ yyvar.i = yytext[0]; return TK_RIGHT_PARENT; }
"["		 							{ yyvar.i = yytext[0]; return TK_LEFT_BRAC; }
"]"		 							{ yyvar.i = yytext[0]; return TK_RIGHT_BRAC; }
"{"		 							{ yyvar.i = yytext[0]; return TK_LEFT_CURV_BRAC; }
"}"		 							{ yyvar.i = yytext[0]; return TK_RIGHT_CURV_BRAC; }
"||"	 							{ yyvar.s = strDup(yytext); return TK_OR;  }
"&&"	 							{ yyvar.s = strDup(yytext); return TK_AND; }
"!"									{ yyvar.i = yytext[0]; return TK_NOT; }
","		 							{ yyvar.i = yytext[0]; return TK_COM; }
";"      							{ yyvar.i = yytext[0]; return TK_SEMIC; }


.									{ errorMsg("Invalid token entry"); return -1; }


%%

/* Função externa para guardar arquivo de leitura */
void setInput (FILE *file) {

	yyin = file;
}

void errorMsg (char *msg) {

	printf("\nError type '%s': in line %d\n", msg, currentLine);
}

void commentError() {

	printf("\nError type 'Comment not closed': from line %d to line %d\n", openComment, currentLine);
}

char charTreat(char first, char second) {

	// Se for escape
	if(first == '\\'){

    	switch(second){
    		case '\'':
    			return '\'';
    			break;
    				
    		case 'n':
    			return '\n';
    			break;
    				
    		case 't':
    			return '\t';
    			break;

    		case '\\':
    			return '\\';
    			break;

    		case '\"':
    			return '\"';
    			break;

    		case '0':
    			return '\0';
    			break;

    		default:
                return -1;
    			break;
    	}
    }

    return first;
}

char * strDup (char *s) {

	int i, j, length;
	char *dup;
    char c;

	length = strlen(s) - 1; // Lembrando: '\0' (+1) menos as duas aspas (-2)

	dup = (char *) malloc (sizeof(char)*length);
    if (dup == NULL) {
    	printf("\nMemory allocation error\n");
    	return NULL; 
    }

    // Eliminando aspas duplas e tratando escapes
    for(i = 1, j = 0; i < (strlen(s) - 1); i++, j++) {

		if(s[i] == '\\') {

			c = charTreat(s[i], s[i+1]);

            if(c == -1) { // Caso de escape indevido

                free(dup);
                return NULL;
            }
            else {
                dup[j] = c;
                i++;
            }
		}
		else
	   		dup[j] = s[i];
    }

    dup[j] = '\0';

    return dup;
}