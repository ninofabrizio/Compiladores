/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_FLOAT = 258,
     TK_CHAR = 259,
     TK_LESS_OR_EQ = 260,
     TK_NOT_EQ = 261,
     TK_OR = 262,
     TK_AND = 263,
     TK_EQUAL = 264,
     TK_ID = 265,
     TK_WORD_CHAR = 266,
     TK_WORD_FLOAT = 267,
     TK_WORD_INT = 268,
     TK_WORD_IF = 269,
     TK_WORD_ELSE = 270,
     TK_WORD_WHILE = 271,
     TK_WORD_NEW = 272,
     TK_WORD_RETURN = 273,
     TK_WORD_VOID = 274,
     TK_LIT_STRING = 275,
     TK_GREATER_OR_EQ = 276,
     TK_INTEGER = 277,
     TK_HEXA = 278
   };
#endif
/* Tokens.  */
#define TK_FLOAT 258
#define TK_CHAR 259
#define TK_LESS_OR_EQ 260
#define TK_NOT_EQ 261
#define TK_OR 262
#define TK_AND 263
#define TK_EQUAL 264
#define TK_ID 265
#define TK_WORD_CHAR 266
#define TK_WORD_FLOAT 267
#define TK_WORD_INT 268
#define TK_WORD_IF 269
#define TK_WORD_ELSE 270
#define TK_WORD_WHILE 271
#define TK_WORD_NEW 272
#define TK_WORD_RETURN 273
#define TK_WORD_VOID 274
#define TK_LIT_STRING 275
#define TK_GREATER_OR_EQ 276
#define TK_INTEGER 277
#define TK_HEXA 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 15 "monga.y"
{
    
    int i;
    double f;
    const char *s;

    AST_Node *node;
    Call *call;
	Param *param;
	
}
/* Line 1529 of yacc.c.  */
#line 107 "monga.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

