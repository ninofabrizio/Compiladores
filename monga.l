%{

#include "ast.h"
#include "monga.tab.h"
#include "monga.h"

void yyerror(char *s);

/* Variável para posição de linha corrente no arquivo de leitura */
int currentLine = 1;

/* Variável para guardar linha onde começou comentário */
int openComment;

/* Função para tratar char */
char charTreat(char first, char second);

/* Função para tratar copia de string */
char * strDup (char *s);

/* Função para notificação de erro de comentário não fechado */
void commentError();

/* Função para extrair string de ID */
char * idDup (char * s);

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


"char"					 			{ return TK_WORD_CHAR; }
"float"								{ return TK_WORD_FLOAT; }
"int"								{ return TK_WORD_INT; }
"if" 								{ return TK_WORD_IF; }
"else" 								{ return TK_WORD_ELSE; }
"while"								{ return TK_WORD_WHILE; }
"new"								{ return TK_WORD_NEW; }
"return"							{ return TK_WORD_RETURN; }
"void"								{ return TK_WORD_VOID; }


({Letter}|_)({Letter}|{Digit}|_)*	        { yylval.s = idDup(yytext); return TK_ID; }

{Digit}+[.]?[Ff]                                        { yylval.f = strtof(yytext, NULL);     return TK_FLOAT; }
({Digit}*[.]{Digit}+)({Exp}[+-]?{Digit}+)?[Ff]?	        { yylval.f = strtof(yytext, NULL);     return TK_FLOAT; }
{Digit}+[.]?						                    { yylval.i = strtol(yytext, NULL, 10); return TK_INTEGER; }
{Digit}+({Exp}[+-]?{Digit}+)[Ff]?						{ yylval.f = strtof(yytext, NULL);     return TK_FLOAT; }
[0][Xx]({Digit}+|[A-Fa-f]+)+							{ yylval.i = strtol(yytext, NULL, 16); return TK_HEXA;  }


('\\')|('')							{ return yytext[0]; }
'{Character}?'						{ yylval.i = charTreat(yytext[1], yytext[2]);
                                      if(yylval.i == -1) {
                                        yylval.s = yytext;
                                        return yytext[0];
                                      }
                                      return TK_CHAR; }


\"\\\"								{ return yytext[0]; }
\"{StringLit}*\"					{ yylval.s = strDup(yytext);
                                      if(yylval.s == NULL) {
                                        yylval.s = yytext;
                                        return yytext[0]; 
                                      }
                                      return TK_LIT_STRING; }


"/*"								{ openComment = currentLine; BEGIN (Comment); }
<Comment>[^*\n]*        			{ ; }
<Comment>"*"+[^*/\n]*   			{ ; }
<Comment>\n 						{ currentLine++; }
<Comment>"*"+"/"        			{ BEGIN(INITIAL); }
<Comment><<EOF>>					{ commentError(); return yytext[0]; }


"=="	 							{ yylval.s = yytext; return TK_EQUAL; }
"!="     							{ yylval.s = yytext; return TK_NOT_EQ; }
"="      							{ return yytext[0]; }
"<="     							{ yylval.s = yytext; return TK_LESS_OR_EQ; }
"<"      							{ return yytext[0]; }
">="     							{ yylval.s = yytext; return TK_GREATER_OR_EQ; }
">"      							{ return yytext[0]; }
"+"      							{ return yytext[0]; }
"-"      							{ return yytext[0]; }
"*"      							{ return yytext[0]; }
"/"      							{ return yytext[0]; }
"("      							{ return yytext[0]; }
")"      							{ return yytext[0]; }
"["		 							{ return yytext[0]; }
"]"		 							{ return yytext[0]; }
"{"		 							{ return yytext[0]; }
"}"		 							{ return yytext[0]; }
"||"	 							{ yylval.s = strDup(yytext); return TK_OR;  }
"&&"	 							{ yylval.s = strDup(yytext); return TK_AND; }
"!"									{ return yytext[0]; }
","		 							{ return yytext[0]; }
";"      							{ return yytext[0]; }


.									{ yylval.s = yytext; return yytext[0]; }


%%

/* Função externa para guardar arquivo de leitura */
void setInput (FILE *file) {

	yyin = file;
}

char * idDup (char * s) {

    int length = strlen(s) + 1;
    char * d = (char *)malloc( length);

    if(d == NULL) 
        fprintf(stderr, "MEMORY SHORTAGE ERROR");
    
    memcpy(d, s, length);

    return d;
}

void commentError() {

	fprintf(stderr, "\nerror type 'Comment not closed': from line %d\n", openComment);
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
    	fprintf(stderr, "\nMemory allocation error in line %d\n", currentLine);
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