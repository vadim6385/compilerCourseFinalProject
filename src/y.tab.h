/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUM = 258,
     ADDOP = 259,
     MULOP = 260,
     ASSIGNOP = 261,
     OROP = 262,
     ANDOP = 263,
     RELOP = 264,
     ID = 265,
     SENTENCE = 266,
     BGN = 267,
     BREAK = 268,
     CASE = 269,
     FINAL = 270,
     DEFAULT = 271,
     DO = 272,
     ELSE = 273,
     END = 274,
     FOREACH = 275,
     IF = 276,
     INT = 277,
     LET = 278,
     OUT = 279,
     REAL = 280,
     READ = 281,
     SWITCH = 282,
     TILL = 283,
     WHILE = 284,
     WITH = 285,
     STRING = 286,
     VAR = 287,
     PROGRAM = 288,
     addop = 289,
     mulop = 290,
     orop = 291,
     andop = 292,
     relop = 293,
     assignop = 294
   };
#endif
/* Tokens.  */
#define NUM 258
#define ADDOP 259
#define MULOP 260
#define ASSIGNOP 261
#define OROP 262
#define ANDOP 263
#define RELOP 264
#define ID 265
#define SENTENCE 266
#define BGN 267
#define BREAK 268
#define CASE 269
#define FINAL 270
#define DEFAULT 271
#define DO 272
#define ELSE 273
#define END 274
#define FOREACH 275
#define IF 276
#define INT 277
#define LET 278
#define OUT 279
#define REAL 280
#define READ 281
#define SWITCH 282
#define TILL 283
#define WHILE 284
#define WITH 285
#define STRING 286
#define VAR 287
#define PROGRAM 288
#define addop 289
#define mulop 290
#define orop 291
#define andop 292
#define relop 293
#define assignop 294



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 390 "cpm.y"

	struct nVal
	{
		enum {I, F} type;
		union
		{
			float fval;
			int   ival;
		}val;
	}nVal;

	char* sval;

	enum {PLUS, MINUS, MUL, DIV, ASSIGN, OR, AND, EQ, NEQ, LT, GT, GTEQ, LTEQ} op;

	struct declaration{
                 int ival;
	             float fval;
	             char *idval;
				 char* type;
	             char *IDarray[5];
	             char *reg;
	             char *label;
	             char *codeHead;
	             char *codeBody;
                 } decl;

	struct mipsCode{
	               char *label;
	               char *head;
	               char *body;
                   } code;


/* Line 2058 of yacc.c  */
#line 170 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;
extern YYLTYPE yylloc;
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
