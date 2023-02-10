/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "cpm.y"

#define _CRT_SECURE_NO_WARNINGS
#define alloca _alloc
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR_STRING_LEN 100
#define STUDENTS_DETAILS "Vadim Darchuk 316920974 and Yotam Alter 302955679\n\n"
#define SYM_TBL_ROWS_COUNT 10

extern int yylex();
extern int yylineno;
void yyerror (char *s);

// ############### Symbol table definitions ####################
typedef enum Type {integer, floating, string} Type;
typedef struct symTblEntry
{
	char* name;
	bool isConst;
	bool isDeclered;
	bool isInit;
	Type type;
	int occurances;
	union{
		int ival;
		float fval;
		char* sval;
	}value;
	struct  symTblEntry* next;
}symTblEntry;

typedef struct symTbl
{
	symTblEntry* symbolTableHead[SYM_TBL_ROWS_COUNT];
}symTbl;


// ############### Symbol table related functions ####################
/*
* Initiates a symbol table for the compilation process
*/
symTbl* initSymTbl()
{
	int i;
	symTbl* symbolTbl = (symTbl*)malloc(sizeof(symTbl));
	if (symbolTbl == NULL)
	{
		fprintf(stderr, "Failed to init a symbol table. Compilation process is terminated\n");
		exit(1);
	}
	
	for (i = 0; i < SYM_TBL_ROWS_COUNT; i++)
		symbolTbl->symbolTableHead[i] = NULL;
	return symbolTbl;
}

/*
* Calculates a hash value for a given lexeme
*/
int getHash(char* lexeme)
{
	if (lexeme == NULL || strlen(lexeme) < 1)
		return -1;
	return strlen(lexeme) % SYM_TBL_ROWS_COUNT;
}


/*
* Searches for an entry in a symbol table in an effective hash-based algorithm
*/
symTblEntry* lookup(symTbl* symTbl, char* lexeme)
{
	if(symTbl == NULL || lexeme == NULL)
		return NULL;

	int hashValue = getHash(lexeme);
	if (hashValue == -1)
	{
		fprintf(stderr, "Failed to get hash value\n");
		return NULL;
	}
	if (hashValue > 0 && hashValue < SYM_TBL_ROWS_COUNT)
	{	
		symTblEntry* ptr = symTbl->symbolTableHead[hashValue];	// lookup only in the line matched  by the hash
		while (ptr != NULL)
		{	
			if (strcmp(ptr->name, lexeme) == 0)
				return ptr;

			ptr = ptr->next;
		}
	}
	return NULL; //requested symbol is not in table
}


/*
* Creates a new symbol instance
*/
symTblEntry* createSymTblEntery(symTbl* symTbl, char * lexeme, Type type, bool isConst, int hashValue)
{
	if (symTbl == NULL || lexeme == NULL || hashValue < 0 || hashValue >= SYM_TBL_ROWS_COUNT)
	{
		fprintf(stderr, "Bad arguments passed to createSymTblEntery\n");
		return NULL;
	}

	symTblEntry* newSymTblEntry = NULL;
	newSymTblEntry = (symTblEntry*)calloc(1, sizeof(newSymTblEntry));
	if (newSymTblEntry == NULL)
		{
			fprintf(stderr, "Failed to allocate memory for a new symbol\n");
			return NULL;
		}

	char* temp;
	temp = strdup(lexeme);
	newSymTblEntry->name = temp;
	newSymTblEntry->isConst = isConst;
	newSymTblEntry->isDeclered = true;
	newSymTblEntry->isInit = false;
	newSymTblEntry->type = type;
	newSymTblEntry->occurances = 1;
	newSymTblEntry->next = NULL;
	return newSymTblEntry;
}


/*
* Add a symbol to the symbol table. Added symbol is added to the head of the appropriate line of the symbol table
*/
void insertEntry(symTbl* symTbl, symTblEntry* newEntry, int hashValue)
{
	if (symTbl == NULL || newEntry == NULL || hashValue < 0 || hashValue >= SYM_TBL_ROWS_COUNT)
	{
		fprintf(stderr, "Bad arguments passed to insertEntry\n");
		return;
	}

	symTblEntry* oldHead = NULL;
	if (symTbl->symbolTableHead[hashValue] == NULL) // the symbol table row is empty
		symTbl->symbolTableHead[hashValue] = newEntry;
		
	else	//otherwise, add newEntry to the Head of the appropriate list
	{
		oldHead = symTbl->symbolTableHead[hashValue];		
		newEntry->next = oldHead;
		symTbl->symbolTableHead[hashValue] = newEntry;
	}
}


/*
* Converts char* type to Enum Type of symbol table
*/
Type charType2EnumType(char* type)
{
	Type newType;		//enum Type {integer, floating, string}
	if(type == NULL)	// for argument protection only
		newType = integer;
	else 
		if(strcmp(type, "int") == 0)
			newType = integer;
	else 
		if (strcmp(type, "float") == 0)
			newType = floating;
	else 
		if (strcmp(type, "string") == 0)
			newType = string;

	return newType;
}


/*
* Converts Enum Type of symbol table to char* type
*/
char* EnumType2charType(Type type)
{
	char* newType;		//enum Type {integer, floating, string}

	if(type == integer)
		newType = strdup("int");
	else
		if(type == floating)
			newType = strdup("float");
	else 
		if (type == string)
			newType = strdup("string");

	return newType;
}

/*
* A wrapper function for adding a new symbol to a symbol table
*/
void addSymbol(symTbl* symTbl, char* lexeme, char* type, bool isConst)
{
	if (symTbl == NULL || lexeme == NULL || type == NULL)
	{
		fprintf(stderr, "Bad arguments passed to addSymbol\n");
		return;
	}

	int hashValue = getHash(lexeme);
	if (hashValue == -1)
	{
		fprintf(stderr, "Bad hash value returned\n");
		return;
	}

	if (lookup(symTbl, lexeme) != NULL) // enetry already in symbol table
	{
		fprintf(stderr, "%s is already in symbol table\n", lexeme);
		return;
	}

	Type newType = charType2EnumType(type);		// Convrting char* type to Enum Type
	symTblEntry* newSymbol = createSymTblEntery(symTbl, lexeme, newType, isConst, hashValue);

	if (newSymbol == NULL)
		return;
	
	insertEntry(symTbl, newSymbol, hashValue);
}


/* 
* Because destroyEntry is only used at the end of the compilation process,
* deletion will be performed from the head of the list to it's tail. 
* It's impossible to delete a middle entry.
* This function DOESN'T handle the linked-list connections. It's the caller's responsibility
*/
void destroyEntry(symTblEntry* entry)
{
	int x;
	if (entry == NULL)
	{
		fprintf(stderr, "Entry to be destroyed is NULL!!\n");
		return;
	}
	if(entry->type == string)
	{
		free(entry->value.sval);
		entry->value.sval = NULL;
	}
	free(entry->name);
	entry->name = NULL;
	entry->next = NULL;
	free(entry);
}


/*
* This function is responsible to destroy the entire symbol table at the end of the compilation process
* The destruction process is done in a sequential order from the head of the list to the tail
* Finally the table itself is destroyed
*/
void destroySymTable(symTbl* symTbl)
{
	symTblEntry* symbol = NULL;
	symTblEntry* nextSymbol = NULL;
	int i;
	for (i = 0; i < SYM_TBL_ROWS_COUNT; i++)
	{
		symbol = symTbl->symbolTableHead[i];
		if (symbol != NULL)	// not an empty row
		{
			while (symbol != NULL)
			{
				nextSymbol = symbol->next;
				destroyEntry(symbol);
				symbol = nextSymbol;
			}
		}
	}
	free(symTbl);
}
// ############### End of symbol table related functions ####################

// ############### Global variables ####################

//symTbl* symbolTable = initSymTbl();
symTbl** ptr2symbolTable = NULL;
char* mipsCode;
bool hasErrors = false;
int nextLabelNum = 0;
char* registerT[8] = {"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7"};	// available MIPS T registers
char* registerF[8] = {"$f0","$f1","$f2","$f3","$f4","$f5","$f6","$f7"}; // available MIPS F registers
int idxT=0;
int idxF=0;
// ############### End of global variables ####################

// ############### Service functions ####################

/*
* Writes an error to listing file
* Since listing file content is written as a whole before compilation proccess,
* this function re-opens listing file and appends the given error string
*/
void outputError(char* s){
     FILE* listFile;
	 listFile = fopen("listing.lst","a+");
     if (listFile == NULL)
     	return;
     else
     {
		 fprintf(listFile,"ERROR in line: %d, %s\n", yylineno, s);
		 fclose(listFile);
     }
}


/*
* Concatenates two strings into one - useful for appending mips code as it is gets written
*/
char* strConcat(char* str1, char* str2)
{
	char* newString =(char*)malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1));
	newString[0] = '\0';
	strcat(newString, str1);
	strcat(newString, str2);
	return newString;
}


/*
* Gets the next available T register for use
*/
char* getRegisterT()
{
	char* res = registerT[idxT];
	idxT++;
	return res;
}



/*
* Frees a given T register
*/
void freeRegisterT(char* reg)
{
	idxT--;
	registerT[idxT] = reg;
}


/*
* Gets the next available F register for use
*/
char* getRegisterF()
{
	char* res = registerF[idxF];
	idxF++;
	return res;
}


/*
* Frees a given T register
*/
void freeRegisterF(char* reg)
{
	idxF--;
	registerF[idxF] = reg;
}


/*
* Generates a unique label
*/
char* getLabel()
{
	char str[100];
	sprintf(str,"Label%d",nextLabelNum++);
	return strdup(str);
}


// ############### End of service functions ###################
// ############### BISON related definitions ##################

/* Line 371 of yacc.c  */
#line 456 "y.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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
     START = 267,
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
     DCL = 278,
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
#define START 267
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
#define DCL 278
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
/* Line 387 of yacc.c  */
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


/* Line 387 of yacc.c  */
#line 612 "y.tab.c"
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

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 653 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   374

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  103
/* YYNRULES -- Number of states.  */
#define YYNSTATES  225

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    47,     2,     2,     2,     2,     2,     2,
      43,    44,     2,     2,    41,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    40,    42,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,     2,    46,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    10,    14,    15,    18,    20,    24,    27,
      31,    34,    36,    39,    41,    43,    45,    53,    60,    61,
      64,    65,    67,    72,    74,    76,    78,    80,    84,    90,
      95,   100,   105,   111,   116,   121,   126,   132,   137,   142,
     147,   152,   156,   164,   171,   178,   184,   189,   194,   199,
     209,   219,   228,   237,   246,   253,   259,   265,   271,   273,
     277,   280,   283,   291,   298,   305,   312,   319,   326,   328,
     330,   338,   345,   352,   356,   359,   365,   370,   375,   380,
     385,   391,   396,   401,   405,   407,   410,   414,   417,   419,
     424,   428,   432,   436,   440,   444,   446,   450,   453,   456,
     458,   462,   465,   467
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      49,     0,    -1,    33,    10,    12,    50,    56,    19,    -1,
      23,    51,    55,    -1,    -1,    51,    52,    -1,    52,    -1,
      54,    40,    53,    -1,    54,    53,    -1,    10,    41,    53,
      -1,    10,    42,    -1,    10,    -1,    10,    53,    -1,    22,
      -1,    25,    -1,    31,    -1,    15,    54,    10,     6,     3,
      42,    55,    -1,    15,    54,    10,     6,     3,    55,    -1,
      -1,    56,    57,    -1,    -1,    60,    -1,    10,     6,    11,
      42,    -1,    61,    -1,    59,    -1,    58,    -1,    62,    -1,
      10,     6,    11,    -1,    24,    43,    70,    44,    42,    -1,
      24,    70,    44,    42,    -1,    24,    43,    70,    42,    -1,
      24,    43,    70,    44,    -1,    24,    43,    11,    44,    42,
      -1,    24,    11,    44,    42,    -1,    24,    43,    11,    42,
      -1,    24,    43,    11,    44,    -1,    26,    43,    10,    44,
      42,    -1,    26,    10,    44,    42,    -1,    26,    43,    10,
      42,    -1,    26,    43,    10,    44,    -1,    10,     6,    70,
      42,    -1,    10,     6,    70,    -1,    21,    43,    67,    44,
      57,    18,    57,    -1,    21,    67,    44,    57,    18,    57,
      -1,    21,    43,    44,    57,    18,    57,    -1,    29,    43,
      67,    44,    62,    -1,    29,    67,    44,    62,    -1,    29,
      43,    44,    62,    -1,    29,    43,    67,    62,    -1,    20,
      10,     6,     3,    40,     3,    30,    66,    57,    -1,    20,
      10,     6,     3,    40,    10,    30,    66,    57,    -1,    20,
      10,     3,    40,     3,    30,    66,    57,    -1,    20,    10,
       6,     3,     3,    30,    66,    57,    -1,    20,    10,     6,
       3,    10,    30,    66,    57,    -1,    17,    62,    28,    43,
      67,    44,    -1,    17,    62,    28,    67,    44,    -1,    17,
      62,    28,    43,    44,    -1,    17,    62,    28,    43,    67,
      -1,    63,    -1,    45,    56,    46,    -1,    56,    46,    -1,
      45,    56,    -1,    27,    43,    64,    44,    45,    65,    46,
      -1,    27,    64,    44,    45,    65,    46,    -1,    27,    43,
      44,    45,    65,    46,    -1,    27,    43,    64,    45,    65,
      46,    -1,    27,    43,    64,    44,    65,    46,    -1,    27,
      43,    64,    44,    45,    65,    -1,    10,    -1,     3,    -1,
      14,     3,    40,    56,    13,    42,    65,    -1,    14,     3,
      56,    13,    42,    65,    -1,    14,     3,    40,    56,    13,
      65,    -1,    16,    40,    56,    -1,    16,    56,    -1,    10,
       6,    10,     4,     3,    -1,    10,    10,     4,     3,    -1,
      10,     6,     4,     3,    -1,    10,     6,    10,     3,    -1,
      10,     6,    10,     4,    -1,    10,     6,    10,     5,     3,
      -1,    10,     6,     5,     3,    -1,    10,     6,    10,     5,
      -1,    67,     7,    68,    -1,    68,    -1,    67,    68,    -1,
      68,     8,    69,    -1,    68,    69,    -1,    69,    -1,    47,
      43,    69,    44,    -1,    47,    69,    44,    -1,    47,    43,
      44,    -1,    47,    43,    69,    -1,    70,     9,    70,    -1,
      70,     4,    71,    -1,    71,    -1,    71,     5,    72,    -1,
      71,    72,    -1,    71,     5,    -1,    72,    -1,    43,    70,
      44,    -1,    43,    70,    -1,    10,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   440,   440,   454,   461,   468,   473,   480,   518,   526,
     548,   561,   562,   569,   574,   579,   586,   614,   622,   629,
     635,   641,   647,   702,   708,   713,   719,   725,   734,   749,
     756,   764,   771,   786,   793,   801,   810,   852,   858,   864,
     872,   932,   941,   956,   963,   970,   988,   995,  1002,  1009,
    1116,  1123,  1130,  1137,  1144,  1162,  1169,  1176,  1183,  1191,
    1197,  1204,  1213,  1219,  1226,  1233,  1240,  1247,  1256,  1290,
    1315,  1329,  1336,  1343,  1355,  1364,  1406,  1413,  1420,  1427,
    1434,  1474,  1481,  1490,  1508,  1515,  1524,  1541,  1548,  1557,
    1571,  1578,  1585,  1592,  1652,  1709,  1719,  1776,  1783,  1790,
    1800,  1808,  1815,  1853
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUM", "ADDOP", "MULOP", "ASSIGNOP",
  "OROP", "ANDOP", "RELOP", "ID", "SENTENCE", "START", "BREAK", "CASE",
  "FINAL", "DEFAULT", "DO", "ELSE", "END", "FOREACH", "IF", "INT", "DCL",
  "OUT", "REAL", "READ", "SWITCH", "TILL", "WHILE", "WITH", "STRING",
  "VAR", "PROGRAM", "addop", "mulop", "orop", "andop", "relop", "assignop",
  "':'", "','", "';'", "'('", "')'", "'{'", "'}'", "'!'", "$accept",
  "program", "DECLARATIONS", "DECLARLIST", "DECL", "LIST", "TYPE", "CDECL",
  "STMTLIST", "STMT", "OUT_STMT", "READ_STMT", "ASSIGNMENT_STMT",
  "CONTROL_STMT", "STMT_BLOCK", "switch", "CHOICE", "CASES", "STEP",
  "BOOLEXPR", "BOOLTERM", "BOOLFACTOR", "EXPRESSION", "TERM", "FACTOR", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      58,    44,    59,    40,    41,   123,   125,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    48,    49,    50,    50,    51,    51,    52,    52,    53,
      53,    53,    53,    54,    54,    54,    55,    55,    55,    56,
      56,    57,    57,    57,    57,    57,    57,    57,    58,    58,
      58,    58,    58,    58,    58,    58,    59,    59,    59,    59,
      60,    60,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    62,
      62,    62,    63,    63,    63,    63,    63,    63,    64,    64,
      65,    65,    65,    65,    65,    66,    66,    66,    66,    66,
      66,    66,    66,    67,    67,    67,    68,    68,    68,    69,
      69,    69,    69,    69,    70,    70,    71,    71,    71,    71,
      72,    72,    72,    72
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     6,     3,     0,     2,     1,     3,     2,     3,
       2,     1,     2,     1,     1,     1,     7,     6,     0,     2,
       0,     1,     4,     1,     1,     1,     1,     3,     5,     4,
       4,     4,     5,     4,     4,     4,     5,     4,     4,     4,
       4,     3,     7,     6,     6,     5,     4,     4,     4,     9,
       9,     8,     8,     8,     6,     5,     5,     5,     1,     3,
       2,     2,     7,     6,     6,     6,     6,     6,     1,     1,
       7,     6,     6,     3,     2,     5,     4,     4,     4,     4,
       5,     4,     4,     3,     1,     2,     3,     2,     1,     4,
       3,     3,     3,     3,     3,     1,     3,     2,     2,     1,
       3,     2,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     1,     4,     0,    20,    13,    14,
      15,    18,     6,     0,    20,     0,     5,     3,    11,     0,
       8,     0,    20,     2,     0,     0,     0,     0,     0,     0,
      20,     0,    19,    25,    24,    21,    23,    26,    58,     0,
       0,    10,    12,     7,     0,     0,     0,   103,   102,     0,
       0,     0,    84,    88,     0,    95,    99,     0,     0,     0,
       0,     0,    69,    68,     0,     0,     0,     0,    61,    60,
       0,     9,    27,     0,    41,     0,     0,     0,    20,     0,
     101,     0,     0,     0,    20,    85,     0,    87,     0,     0,
      98,    97,     0,     0,   101,     0,     0,     0,     0,     0,
       0,    20,    20,    20,    59,     0,    22,   101,    40,     0,
       0,     0,     0,     0,    20,   100,    91,    92,    90,    83,
       0,    86,    94,    93,    96,    33,    34,    35,    30,    31,
      29,    37,    38,    39,     0,     0,     0,     0,    47,    20,
      48,    46,    18,    56,    57,    55,     0,     0,     0,     0,
      20,     0,    89,    20,    32,    28,    36,     0,    20,     0,
       0,     0,     0,     0,    45,    18,    17,    54,     0,     0,
       0,     0,     0,    44,    20,    43,    20,    20,    74,    64,
      67,    66,    65,    63,    16,     0,    20,    20,    20,     0,
       0,    42,    20,    20,    73,    62,     0,     0,    51,    52,
      53,    20,    20,    20,     0,     0,     0,     0,     0,    49,
      50,     0,     0,    77,    81,    78,    79,    82,    76,     0,
      72,    71,    75,    80,    70
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     7,    11,    12,    20,    13,    17,    31,    32,
      33,    34,    35,    36,    37,    38,    65,   159,   186,    51,
      52,    53,    54,    55,    56
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -128
static const yytype_int16 yypact[] =
{
      15,    92,    18,   105,  -128,   166,   269,  -128,  -128,  -128,
    -128,   184,  -128,   106,   242,   269,  -128,  -128,   278,   194,
    -128,   202,   174,  -128,   203,    60,   173,   102,   123,   134,
    -128,   197,  -128,  -128,  -128,  -128,  -128,  -128,  -128,   215,
     194,  -128,  -128,  -128,   177,   206,   159,  -128,  -128,    85,
     139,    14,   110,  -128,   127,   188,  -128,   150,   192,    56,
     196,   240,  -128,  -128,   112,   211,    96,    22,   212,  -128,
     254,  -128,   222,   187,     8,   149,   225,   273,   287,    31,
      11,   111,   231,   158,   287,   110,   158,  -128,   187,   187,
     187,  -128,   237,   -14,    35,   238,   248,   292,   250,   201,
     261,   174,    46,   174,  -128,   309,  -128,   156,  -128,   120,
      61,   315,   138,   303,   287,  -128,  -128,   282,  -128,   110,
     322,  -128,   188,   334,  -128,  -128,  -128,   297,  -128,   168,
    -128,  -128,  -128,   299,   321,     0,   321,   321,  -128,   174,
    -128,  -128,   132,  -128,    91,  -128,   313,   314,   316,   312,
     287,   324,  -128,   287,  -128,  -128,  -128,   342,   307,   302,
     321,   304,   305,   306,  -128,   338,  -128,  -128,   339,   339,
     339,   325,   326,  -128,   287,  -128,   317,  -128,   227,  -128,
     308,  -128,  -128,  -128,  -128,   169,   287,   287,   287,   339,
     339,  -128,  -128,   257,   227,  -128,   323,   354,  -128,  -128,
    -128,   287,   287,   272,   318,   356,   358,   320,   359,  -128,
    -128,   289,   321,  -128,  -128,  -128,   360,   361,  -128,   321,
    -128,  -128,  -128,  -128,  -128
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -128,  -128,  -128,  -128,   355,   209,   350,   -15,    -7,   -77,
    -128,  -128,  -128,  -128,   -20,  -128,   310,  -127,   140,   -23,
     -48,   -39,   -22,   279,   -50
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -101
static const yytype_int16 yytable[] =
{
      14,   113,    45,    85,    59,    91,    67,   120,   161,   162,
     163,    82,    88,    87,   157,    88,   158,    47,     4,    85,
      89,    83,    74,    68,    48,    47,    79,    80,   126,    83,
     127,    85,    48,   180,    47,   119,    94,   151,    83,    88,
     124,    48,   117,   102,    80,   160,    87,   121,     1,    47,
     108,   107,   110,    83,    85,   115,    48,    73,    84,    80,
      88,    50,    85,    47,    47,    73,   103,   123,    83,    50,
      48,    48,    91,   173,    73,   114,   175,   128,    50,   129,
      87,   138,   140,   141,   220,   221,   144,    80,    47,    73,
     139,    30,   224,    50,    47,    48,    85,   191,    83,    47,
      95,    48,     3,    49,    73,   145,    48,    50,    50,   198,
     199,   200,    60,    47,    47,    62,    18,     5,    86,   164,
      48,    48,    63,    47,   209,   210,    62,   166,    73,    78,
      48,    88,    50,    63,    73,   167,    89,    47,    50,    73,
     101,   147,    47,    50,    48,    61,    19,    15,   148,    48,
     184,   178,    47,    73,    73,   116,    98,    50,    50,    48,
      88,    47,    76,    73,   143,    77,    64,    50,    48,   193,
     194,  -100,  -100,  -100,   165,   196,    47,    66,   149,   197,
      47,    50,    81,    48,    57,   203,    50,    48,    72,     6,
      47,    47,   109,    90,    92,    47,    50,    48,    48,    15,
     115,    73,    48,    93,    18,    50,     8,    21,    44,     9,
     155,  -100,  -100,    46,    22,    10,    58,    24,    25,    30,
      73,    26,    21,    27,    28,    70,    29,    42,    43,    22,
      73,    73,    24,    25,    75,    73,    26,    21,    27,    28,
      96,    29,    30,    69,    22,   135,   136,    24,    25,    71,
      97,    26,    21,    27,    28,   100,    29,    30,   104,    22,
     105,    23,    24,    25,   106,   111,    26,    21,    27,    28,
     204,    29,    30,   -20,    22,   118,   112,    24,    25,   125,
     130,    26,    21,    27,    28,   211,    29,    30,    18,    22,
     131,     8,    24,    25,     9,   134,    26,    21,    27,    28,
      10,    29,    30,   157,    22,   158,   137,    24,    25,   187,
     188,    26,   142,    27,    28,   171,    29,    30,   146,    40,
      41,   150,   172,   215,   216,   217,   152,   205,   206,   201,
     202,   219,    30,   207,   132,   157,   133,   158,    88,   154,
     153,   156,   174,   168,   169,   176,   170,   177,   179,   185,
     181,   182,   183,    15,   195,   189,   190,   192,   208,   213,
     212,   214,   218,   222,   223,    39,    16,   122,     0,     0,
       0,     0,     0,     0,    99
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-128)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       7,    78,    22,    51,    26,    55,    29,    84,   135,   136,
     137,    50,     4,    52,    14,     4,    16,     3,     0,    67,
       9,     7,    44,    30,    10,     3,    49,    49,    42,     7,
      44,    79,    10,   160,     3,    83,    58,   114,     7,     4,
      90,    10,    81,    66,    66,    45,    85,    86,    33,     3,
      42,    73,    75,     7,   102,    44,    10,    43,    44,    81,
       4,    47,   110,     3,     3,    43,    44,    89,     7,    47,
      10,    10,   122,   150,    43,    44,   153,    42,    47,    44,
     119,   101,   102,   103,   211,   212,   109,   109,     3,    43,
      44,    45,   219,    47,     3,    10,   144,   174,     7,     3,
      44,    10,    10,    43,    43,    44,    10,    47,    47,   186,
     187,   188,    10,     3,     3,     3,    10,    12,     8,   139,
      10,    10,    10,     3,   201,   202,     3,   142,    43,    44,
      10,     4,    47,    10,    43,    44,     9,     3,    47,    43,
      44,     3,     3,    47,    10,    43,    40,    15,    10,    10,
     165,   158,     3,    43,    43,    44,    44,    47,    47,    10,
       4,     3,     3,    43,    44,     6,    43,    47,    10,   176,
     177,     3,     4,     5,    42,     6,     3,    43,    40,    10,
       3,    47,    43,    10,    11,   192,    47,    10,    11,    23,
       3,     3,    43,     5,    44,     3,    47,    10,    10,    15,
      44,    43,    10,    11,    10,    47,    22,    10,     6,    25,
      42,    43,    44,    10,    17,    31,    43,    20,    21,    45,
      43,    24,    10,    26,    27,    10,    29,    18,    19,    17,
      43,    43,    20,    21,    28,    43,    24,    10,    26,    27,
      44,    29,    45,    46,    17,    44,    45,    20,    21,    40,
      10,    24,    10,    26,    27,    44,    29,    45,    46,    17,
       6,    19,    20,    21,    42,    40,    24,    10,    26,    27,
      13,    29,    45,    46,    17,    44,     3,    20,    21,    42,
      42,    24,    10,    26,    27,    13,    29,    45,    10,    17,
      42,    22,    20,    21,    25,    45,    24,    10,    26,    27,
      31,    29,    45,    14,    17,    16,    45,    20,    21,   169,
     170,    24,     3,    26,    27,     3,    29,    45,     3,    41,
      42,    18,    10,     3,     4,     5,    44,     4,     5,   189,
     190,    42,    45,    10,    42,    14,    44,    16,     4,    42,
      18,    42,    18,    30,    30,     3,    30,    40,    46,    10,
      46,    46,    46,    15,    46,    30,    30,    40,     4,     3,
      42,     3,     3,     3,     3,    15,    11,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    33,    49,    10,     0,    12,    23,    50,    22,    25,
      31,    51,    52,    54,    56,    15,    52,    55,    10,    40,
      53,    10,    17,    19,    20,    21,    24,    26,    27,    29,
      45,    56,    57,    58,    59,    60,    61,    62,    63,    54,
      41,    42,    53,    53,     6,    62,    10,     3,    10,    43,
      47,    67,    68,    69,    70,    71,    72,    11,    43,    70,
      10,    43,     3,    10,    43,    64,    43,    67,    56,    46,
      10,    53,    11,    43,    70,    28,     3,     6,    44,    67,
      70,    43,    69,     7,    44,    68,     8,    69,     4,     9,
       5,    72,    44,    11,    70,    44,    44,    10,    44,    64,
      44,    44,    67,    44,    46,     6,    42,    70,    42,    43,
      67,    40,     3,    57,    44,    44,    44,    69,    44,    68,
      57,    69,    71,    70,    72,    42,    42,    44,    42,    44,
      42,    42,    42,    44,    45,    44,    45,    45,    62,    44,
      62,    62,     3,    44,    67,    44,     3,     3,    10,    40,
      18,    57,    44,    18,    42,    42,    42,    14,    16,    65,
      45,    65,    65,    65,    62,    42,    55,    44,    30,    30,
      30,     3,    10,    57,    18,    57,     3,    40,    56,    46,
      65,    46,    46,    46,    55,    10,    66,    66,    66,    30,
      30,    57,    40,    56,    56,    46,     6,    10,    57,    57,
      57,    66,    66,    56,    13,     4,     5,    10,     4,    57,
      57,    13,    42,     3,     3,     3,     4,     5,     3,    42,
      65,    65,     3,     3,    65
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 441 "cpm.y"
    {// 0		1	  2	  3     4			5		6
	printf("PROGRAM --> PROGRAM ID START DECLARATIONS STMTLIST END\n");
	char* str = (char*)calloc(1000, sizeof(char));
	
	if((yyvsp[(5) - (6)].code).head == NULL)
	(yyvsp[(5) - (6)].code).head = "";
	if((yyvsp[(4) - (6)].decl).codeBody == NULL)
	(yyvsp[(4) - (6)].decl).codeBody = "";
	sprintf(str,".data\n%s\n%s\nstrBuff: .space 200\n.text\nProgram:\n%s\n%s\n", (yyvsp[(4) - (6)].decl).codeHead, (yyvsp[(5) - (6)].code).head, (yyvsp[(4) - (6)].decl).codeBody, (yyvsp[(5) - (6)].code).body);
	mipsCode = str;
}
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 455 "cpm.y"
    {//0			1		2		3
	printf("DECLARATIONS --> DCL DECLARLIST CDECL\n");
	(yyval.decl).codeHead = strdup(strcat((yyvsp[(2) - (3)].decl).codeHead, (yyvsp[(3) - (3)].decl).codeHead));
	(yyval.decl).codeBody = strdup((yyvsp[(3) - (3)].decl).codeBody);
}
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 461 "cpm.y"
    {
	printf("DECLARATIONS --> epsilon\n");
	(yyval.decl).codeHead = strdup("");
	(yyval.decl).codeBody = strdup("");
}
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 469 "cpm.y"
    {
	printf("DECLARLIST --> DECLARLIST DECL\n");
	(yyval.decl).codeHead = strConcat((yyvsp[(1) - (2)].decl).codeHead, (yyvsp[(2) - (2)].decl).codeHead);
}
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 474 "cpm.y"
    {
	printf("DECLARLIST --> DECL\n");
	(yyval.decl).codeHead = strdup((yyvsp[(1) - (1)].decl).codeHead);
}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 481 "cpm.y"
    {//0	1	  2		3
	printf("DECL -->  TYPE ':' LIST\n");
	char* tempCodeHeadStr = (char*)calloc(200, sizeof(char));
	char* codeHeadStr = (char*)calloc(200, sizeof(char));
	char errorMessage[ERROR_STRING_LEN]; 
	int i;
	
	for(i = 0 ; i < 5 ; i++)
	{
		if((yyvsp[(3) - (3)].decl).IDarray[i] != NULL)
		{
			if(lookup(*ptr2symbolTable, (yyvsp[(3) - (3)].decl).IDarray[i]) == NULL)
			{
				addSymbol(*ptr2symbolTable, (yyvsp[(3) - (3)].decl).IDarray[i], (yyvsp[(1) - (3)].decl).type, false);
				if(strcmp((yyvsp[(1) - (3)].decl).type,"string") == 0)
				{
					sprintf(tempCodeHeadStr,"%s: .space 200\n",(yyvsp[(3) - (3)].decl).IDarray[i]);
					strcat(codeHeadStr,tempCodeHeadStr);
				}
				else
				{
					sprintf(tempCodeHeadStr,"%s: .space 8\n",(yyvsp[(3) - (3)].decl).IDarray[i]);
					strcat(codeHeadStr,tempCodeHeadStr);
				}
			}
			else 
			{	sprintf(errorMessage, "Duplicted declaration. %s already declared\n", (yyvsp[(3) - (3)].decl).IDarray[i]);
				outputError(errorMessage);
				sprintf(codeHeadStr,"");
				hasErrors = true;
			}
		}
	}
	(yyval.decl).codeHead = strdup(codeHeadStr);
	free(codeHeadStr);
	free(tempCodeHeadStr);
}
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 519 "cpm.y"
    {
outputError("Expected ':' ");
hasErrors = true;
(yyval.decl).codeHead = strdup("");
}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 527 "cpm.y"
    {//0	1  2	3
	printf("LIST --> ID ',' LIST\n");
	int i, j;
	for(i = 0 ; i < 5 ; i++)	//Assuming up to 5 Ids in a chained declaration
	{
		if((yyval.decl).IDarray[i] == NULL)
		{
			(yyval.decl).IDarray[i] = (yyvsp[(1) - (3)].sval);
				for(j = 0 ; j < 5 ; j++)
				{
					if((yyvsp[(3) - (3)].decl).IDarray[i] != NULL)
					{
						(yyval.decl).IDarray[i+1] = (yyvsp[(3) - (3)].decl).IDarray[j];
						i++;
					}
				}
			break;
		}
	}
}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 549 "cpm.y"
    {//1
	printf("LIST --> ID ';'\n");
	int i;
	for(i = 0 ; i < 5 ; i++)	//Assuming up to 5 Ids in a chained declaration
	{
		if((yyval.decl).IDarray[i] == NULL)
		{
			(yyval.decl).IDarray[i] = (yyvsp[(1) - (2)].sval);
			break;
		}
	}
}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 561 "cpm.y"
    { outputError("Expected ';' "); hasErrors = true; }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 563 "cpm.y"
    {
	outputError("Expected ',' ");
	hasErrors = true;
}
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 570 "cpm.y"
    {//0    1
	printf("TYPE --> INT\n");
	(yyval.decl).type = strdup("int");
}
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 575 "cpm.y"
    {//1
	printf("TYPE --> REAL\n");
	(yyval.decl).type = strdup("float");
}
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 580 "cpm.y"
    {//1
	printf("TYPE --> STRING\n");
	(yyval.decl).type = strdup("string");
}
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 587 "cpm.y"
    {//0	1		2	3	4		5  6	7
	printf("CDECL --> FINAL TYPE ID ASSIGNOP NUM ';' CDECL\n");
	char* codeBodyStr = (char*)calloc(200, sizeof(char));
	char* codeHeadStr = (char*)calloc(200, sizeof(char));
	char errorMessage[ERROR_STRING_LEN];
	if(lookup(*ptr2symbolTable, (yyvsp[(3) - (7)].sval)) == NULL) //id not in symbol table
	{
		addSymbol(*ptr2symbolTable, (yyvsp[(3) - (7)].sval), (yyvsp[(2) - (7)].decl).type, true);
		if(strcmp((yyvsp[(2) - (7)].decl).type,"string")==0)
			sprintf(codeHeadStr,"%s: .space 200\n",(yyvsp[(3) - (7)].sval));
		else
		{
			sprintf(codeHeadStr,"%s: .space 8\n",(yyvsp[(3) - (7)].sval));
			if(strcmp((yyvsp[(2) - (7)].decl).type,"int")==0)
				sprintf(codeBodyStr,"li $t0,%d\nsw $t0, %s\n",(yyvsp[(5) - (7)].nVal).val,(yyvsp[(3) - (7)].sval));
		}
	}
	else
	{
		sprintf(errorMessage, "Duplicted declaration. %s already declared\n", (yyvsp[(3) - (7)].sval));
		outputError(errorMessage);
		sprintf(codeHeadStr,"");
		hasErrors = true;
	}
	(yyval.decl).codeHead = strConcat(codeHeadStr, (yyvsp[(7) - (7)].decl).codeHead);
	(yyval.decl).codeBody = strConcat(codeBodyStr, (yyvsp[(7) - (7)].decl).codeBody);
}
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 615 "cpm.y"
    {
	outputError("Expected ';' ");
	hasErrors = true;
	(yyval.decl).codeHead = strdup("");
	(yyval.decl).codeBody = strdup("");
}
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 622 "cpm.y"
    {
	printf("CDECL --> epsilon\n");
	(yyval.decl).codeHead = strdup("");
	(yyval.decl).codeBody = strdup("");
}
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 630 "cpm.y"
    {//0		1		 2
	printf("STMTLIST --> STMTLIST STMT\n");
	(yyval.code).body = strConcat((yyvsp[(1) - (2)].code).body, (yyvsp[(2) - (2)].code).body);
}
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 635 "cpm.y"
    {
	printf("STMTLIST --> epsilon\n");
	(yyval.code).body = strdup("");
}
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 642 "cpm.y"
    {//0	1
	printf("STMT --> ASSIGNMENT_STMT\n");
	(yyval.code).head = strdup((yyvsp[(1) - (1)].code).head);
	(yyval.code).body = strdup((yyvsp[(1) - (1)].code).body);
}
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 648 "cpm.y"
    {//1	2		3		4
	printf("STMT --> ID ASSIGNOP SENTENCE ';'\n");
	
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* codeHeadStr = (char*)calloc(200,sizeof(char));
	char* temp;
	char errorMessage[ERROR_STRING_LEN];

	char* reg = getRegisterT();
	char* label = getLabel();
	symTblEntry*  symbol = lookup(*ptr2symbolTable, (yyvsp[(1) - (4)].sval));
	if (symbol != NULL)
	{	
		if(symbol->isConst == false)
		{
			if(symbol->type == string)
			{	
				temp = (char*)calloc(1, sizeof(char) * (strlen((yyvsp[(3) - (4)].sval))+1));
				temp = strcpy(temp, (yyvsp[(3) - (4)].sval));
				symbol->value.sval = temp;
				
				sprintf(codeHeadStr,"%s: .asciiz %s\n", label, (yyvsp[(3) - (4)].sval));
				sprintf(codeBodyStr,"lw %s, %s\nsw %s, %s\n", reg, label, reg, (yyvsp[(1) - (4)].sval));
			}
			else
			{
				sprintf(errorMessage, "Trying to assign sentence to an id which isn't of type string");
				outputError(errorMessage);
				sprintf(codeBodyStr,"");
				hasErrors = true;
			}
		}
		else
		{
			sprintf(errorMessage, "Can't assign to a constant variable");
			outputError(errorMessage);
			sprintf(codeBodyStr,"");
			hasErrors = true;
			}
	}
	else
	{
		sprintf(errorMessage,"Id isn't declared");
		outputError(errorMessage);
		sprintf(codeBodyStr,"");
		hasErrors = true;
	}

	freeRegisterT(reg);
	(yyval.code).head = strdup(codeHeadStr);
	(yyval.code).body = strdup(codeBodyStr);
	free(codeHeadStr);
	free(codeBodyStr);
}
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 703 "cpm.y"
    {	//1
	printf("STMT --> CONTROL_STMT\n");
	(yyval.code).head = strdup((yyvsp[(1) - (1)].code).head);
	(yyval.code).body = strdup((yyvsp[(1) - (1)].code).body);
}
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 709 "cpm.y"
    { //1
	printf("STMT --> READ_STMT\n");
	(yyval.code).body = strdup((yyvsp[(1) - (1)].code).body);
}
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 714 "cpm.y"
    { //1 
	printf("STMT --> OUT_STMT\n");
	(yyval.code).head = strdup((yyvsp[(1) - (1)].code).head);
	(yyval.code).body = strdup((yyvsp[(1) - (1)].code).body);
}
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 720 "cpm.y"
    { //1
	printf("STMT --> STMT_BLOCK\n");
	(yyval.code).head = strdup((yyvsp[(1) - (1)].code).head);
	(yyval.code).body = strdup((yyvsp[(1) - (1)].code).body);
}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 726 "cpm.y"
    { 
	outputError("Expected ';' ");
	hasErrors = true;
	(yyval.code).head = strdup("");
	(yyval.code).body = strdup("");
}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 735 "cpm.y"
    {//0		1	2	3		   4   5
	printf("OUT_STMT --> OUT '(' EXPRESSION ')' ';'\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	
	if(strcmp((yyvsp[(3) - (5)].decl).type, "int") == 0)	//print an int
		sprintf(codeBodyStr,"li $v0,1\nmove $a0,%s\n syscall\n",(yyvsp[(3) - (5)].decl).reg);
	
	if(strcmp((yyvsp[(3) - (5)].decl).type, "float") == 0)	//print a float
		sprintf(codeBodyStr,"li $v0,2\nmov.s $f12,%s\n syscall\n",(yyvsp[(3) - (5)].decl).reg);
	
	(yyval.code).head = (yyvsp[(3) - (5)].decl).codeHead;
	(yyval.code).body = strConcat((yyvsp[(3) - (5)].decl).codeBody, codeBodyStr);
	free(codeBodyStr);
}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 750 "cpm.y"
    {
	outputError("Expected '(' ");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 757 "cpm.y"
    {
	outputError("Expected ')'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 765 "cpm.y"
    { 
	outputError("expected ';' ");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 772 "cpm.y"
    {//1	2	3		4	5
	printf("OUT_STMT --> OUT '(' SENTENCE ')' ';'\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* codeHeadStr = (char*)calloc(200,sizeof(char));
	char* label = getLabel();
	
	sprintf(codeHeadStr, "%s: .asciiz %s\n", label, (yyvsp[(3) - (5)].sval));
	sprintf(codeBodyStr, "la $a0,%s\nli $v0,4\nsyscall\n", label); //print a string
	
	(yyval.code).head = strdup(codeHeadStr);
	(yyval.code).body = strdup(codeBodyStr);
	free(codeHeadStr);
	free(codeBodyStr);
}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 787 "cpm.y"
    {
	outputError("Expected '('");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 794 "cpm.y"
    {
	outputError("Expected ')'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 802 "cpm.y"
    {
	outputError("expected ';'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 811 "cpm.y"
    {//0		1	  2	  3	 4	 5
	printf("READ_STMT --> READ '(' ID ')' ';'\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char errorMessage[ERROR_STRING_LEN];

	symTblEntry*  symbol = lookup(*ptr2symbolTable, (yyvsp[(3) - (5)].sval));
	if (symbol != NULL)
	{
		if(symbol->isConst == false)
		{
			if(symbol->type == integer)	//read int
				sprintf(codeBodyStr,"li $v0,5\nsyscall\nsw $v0, %s\n", symbol->name);

			if(symbol->type == floating) //read float
				sprintf(codeBodyStr,"li $v0,6\nsyscall\ns.s $f0, %s\n", symbol->name);

			if(symbol->type == string)	//read string
			{
				strcat(codeBodyStr,"li $v0,8\nla $a0,");
				strcat(codeBodyStr,symbol->name);
				strcat(codeBodyStr,"\nli $a1,200\nsyscall\n");
			}
		}
		else
		{
			sprintf(errorMessage,"Can't assign to a constant");
			outputError(errorMessage);
			sprintf(codeBodyStr,"");
			hasErrors = true;
		}
	}
	else
	{
		sprintf(errorMessage,"Id isn't declared");
		outputError(errorMessage);
		sprintf(codeBodyStr,"");
		hasErrors = true;
	}
	(yyval.code).body = strdup(codeBodyStr);
	free(codeBodyStr);
}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 853 "cpm.y"
    {
	outputError("expected '('");
	hasErrors = true;
	(yyval.code).body=strdup("");
}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 859 "cpm.y"
    {
	outputError("expected ')'");
	hasErrors = true;
	(yyval.code).body=strdup("");
}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 865 "cpm.y"
    {
	outputError("expected ';'");
	hasErrors = true;
	(yyval.code).body=strdup("");
}
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 873 "cpm.y"
    {//0			  1		2		3		 4
	printf("ASSIGNMENT_STMT --> ID ASSIGNOP EXPRESSION';'\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char errorMessage[ERROR_STRING_LEN];
	char* reg;

	symTblEntry*  symbol = lookup(*ptr2symbolTable, (yyvsp[(1) - (4)].sval));
	if (symbol != NULL)
	{
		if(symbol->isConst == false)
		{ 
			if(strcmp((yyvsp[(3) - (4)].decl).type,"int") == 0 && symbol->type == integer) //ints assignment
			{
				sprintf(codeBodyStr,"sw %s, %s\n", (yyvsp[(3) - (4)].decl).reg, (yyvsp[(1) - (4)].sval));
				freeRegisterT((yyvsp[(3) - (4)].decl).reg);
			}	
			else if(strcmp((yyvsp[(3) - (4)].decl).type,"float") == 0 && symbol->type == floating) //floats assignment
			{
				sprintf(codeBodyStr,"s.s %s, %s\n", (yyvsp[(3) - (4)].decl).reg, (yyvsp[(1) - (4)].sval));
				freeRegisterF((yyvsp[(3) - (4)].decl).reg);
			}
				else
				{
					if(strcmp((yyvsp[(3) - (4)].decl).type,"int")==0 && symbol->type == floating) //assign an int into a float
					{
						reg = getRegisterF();
						sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ns.s %s, %s\n", (yyvsp[(3) - (4)].decl).reg, reg, reg, reg, reg, (yyvsp[(1) - (4)].sval));
						freeRegisterT((yyvsp[(3) - (4)].decl).reg);
						freeRegisterF(reg);
					}
					else	// attempting to assign a float into an int
					{
						sprintf(errorMessage,"Can't convert from a real number to integer. Illegal assignment");
						outputError(errorMessage);
						sprintf(codeBodyStr,"");
						hasErrors = true;
					}
				}
		}
		else
		{
			sprintf(errorMessage,"Can't assign to a constant");
			outputError(errorMessage);
			sprintf(codeBodyStr,"");
			hasErrors = true;
		}
	}
	else
	{
		sprintf(errorMessage,"Id isn't declared");
		outputError(errorMessage);
		sprintf(codeBodyStr,"");
		hasErrors = true;
	}

	(yyval.code).head = strdup((yyvsp[(3) - (4)].decl).codeHead);
	(yyval.code).body = strConcat((yyvsp[(3) - (4)].decl).codeBody, codeBodyStr);
	free(codeBodyStr);
}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 933 "cpm.y"
    {
	outputError("expected ';'");
	hasErrors = true;
	(yyval.code).head = strdup("");
	(yyval.code).body = strdup("");
}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 942 "cpm.y"
    {//0			1	2	3		4	5	 6	  7
	printf("CONTROL_STMT --> IF '(' BOOLEXPR ')' STMT ELSE STMT\n");
	char* codeBodyStr = (char*)calloc((200 + strlen((yyvsp[(5) - (7)].code).body)+ strlen((yyvsp[(7) - (7)].code).body)),sizeof(char));
	char* label = getLabel();
	char* str;
	
	sprintf(codeBodyStr,"beq %s,$0,Else%s\n%s\nj End%s\nElse%s:\n%s\nEnd%s:\n", (yyvsp[(3) - (7)].decl).reg, label, (yyvsp[(5) - (7)].code).body, label, label ,(yyvsp[(7) - (7)].code).body, label);	
	str = strConcat((yyvsp[(5) - (7)].code).head, (yyvsp[(7) - (7)].code).head);
	(yyval.code).head = strConcat((yyvsp[(3) - (7)].decl).codeHead, str);
	(yyval.code).body = strConcat((yyvsp[(3) - (7)].decl).codeBody, codeBodyStr);
	freeRegisterT((yyvsp[(3) - (7)].decl).reg);
	free(str);
	free(codeBodyStr);
}
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 957 "cpm.y"
    {
	outputError("expected '('");
	hasErrors = true;
	(yyval.code).body=strdup("");
	(yyval.code).head=strdup("");
}
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 964 "cpm.y"
    {
	outputError("Expected a boolean expression");
	hasErrors = true;
	(yyval.code).body=strdup("");
	(yyval.code).head=strdup("");
}
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 971 "cpm.y"
    {//1	 2		3	  4		5
	printf("CONTROL_STMT --> WHILE '(' BOOLEXPR ')' STMT_BLOCK\n");
	char* codeBodyStr = (char*)calloc((200 + strlen((yyvsp[(5) - (5)].code).body)),sizeof(char));
	char* label = getLabel();
	char* str = (char*)calloc((200 + strlen((yyvsp[(3) - (5)].decl).codeBody)),sizeof(char));
	codeBodyStr[0] = '\0';
	
	sprintf(codeBodyStr,"beq %s, $0, End%s:\n%s\nj Loop%s:\nEnd%s:\n", (yyvsp[(3) - (5)].decl).reg, label, (yyvsp[(5) - (5)].code).body, label, label);
	sprintf(str,"Loop%s:\n", label);
	strcat(str, (yyvsp[(3) - (5)].decl).codeBody);

	(yyval.code).head = strConcat((yyvsp[(3) - (5)].decl).codeHead, (yyvsp[(5) - (5)].code).head);
	(yyval.code).body = strConcat(str, codeBodyStr);
	freeRegisterT((yyvsp[(3) - (5)].decl).reg);
	free(str);
	free(codeBodyStr);
}
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 989 "cpm.y"
    {
	outputError("expected '('");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");	
}
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 996 "cpm.y"
    {
	outputError("Expected a boolean expression");
	hasErrors = true; 
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 1003 "cpm.y"
    {
	outputError("expected ')'");
	hasErrors = true; 
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 1010 "cpm.y"
    {//1	  2		3		4  5   6   7    8    9
	printf("CONTROL_STMT --> FOREACH ID ASSIGNOP NUM ':' NUM WITH STEP STMT\n");
	char* codeBodyStr = (char*)calloc((200 + strlen((yyvsp[(8) - (9)].code).body) + strlen((yyvsp[(9) - (9)].code).body)),sizeof(char));
	char* str = (char*)calloc((200 + strlen((yyvsp[(8) - (9)].code).body) + strlen((yyvsp[(9) - (9)].code).body)),sizeof(char));
	char errorMessage[ERROR_STRING_LEN];
	char* label = getLabel();
	char* reg;
	char* reg1;
	char* reg2;
	char* reg3;
	bool assignDone = false;
	
	symTblEntry*  symbol = lookup(*ptr2symbolTable, (yyvsp[(2) - (9)].sval));
	if (symbol != NULL)
	{
		if(symbol->isConst == false)
		{ 
			if((yyvsp[(4) - (9)].nVal).type == I && symbol->type == integer  && (yyvsp[(6) - (9)].nVal).type == I) //ints assignment
			{
				reg1 = getRegisterT();
				reg2 = getRegisterT();
				sprintf(codeBodyStr,"la %s,$s\naddi $s,$0,$d\nsw %s,0(%s)\n", reg1, (yyvsp[(2) - (9)].sval), reg2, (yyvsp[(4) - (9)].nVal).val.ival,reg2, reg1);
				freeRegisterT(reg1);
				freeRegisterT(reg2);
				assignDone = true;
			}	
			else
			{
				if((yyvsp[(4) - (9)].nVal).type == F && symbol->type == floating && (yyvsp[(6) - (9)].nVal).type == F) //floats assignment
				{
					reg1 = getRegisterF();
					reg2 = getRegisterF();
					sprintf(codeBodyStr,"la %s,$s\naddi $s,$0,$f\ns.s %s, %s\n", reg1, (yyvsp[(2) - (9)].sval), reg2, (yyvsp[(4) - (9)].nVal).val.fval, reg2, reg1);
					freeRegisterF(reg1);
					freeRegisterF(reg2);
					assignDone = true;
				}
				else
				{
					if((yyvsp[(4) - (9)].nVal).type == I && symbol->type == floating && (yyvsp[(6) - (9)].nVal).type == I) //assign an int into a float
					{
						reg1 = getRegisterT();
						reg2 = getRegisterT();
						reg3 = getRegisterF();
						sprintf(codeBodyStr,"la %s,$s\naddi $s,$0,$d\nmtc1 %s, %s\ncvt.s.w %s, %s\ns.s %s, %s\n", reg1, (yyvsp[(2) - (9)].sval), reg2, (yyvsp[(4) - (9)].nVal).val.ival, reg2, reg3, reg3 ,reg3,reg3,reg1);
						freeRegisterT(reg1);
						freeRegisterT(reg2);
						freeRegisterF(reg3);
						assignDone = true;
					}
					else	// attempting to assign a float into an int
					{
						sprintf(errorMessage,"Can't convert from a real number to integer");
						outputError(errorMessage);
						sprintf(codeBodyStr,"");
						hasErrors = true;
					}
				}
			}
		}
		else
		{
			sprintf(errorMessage,"Can't assign to a constant");
			outputError(errorMessage);
			sprintf(codeBodyStr,"");
			hasErrors = true;
		}
	}
	else
	{
		sprintf(errorMessage,"Id isn't declared");
		outputError(errorMessage);
		sprintf(codeBodyStr,"");
		hasErrors = true;
	}

	if (assignDone == true)
	{
		// assignment completed correctly
		if ((yyvsp[(6) - (9)].nVal).type == I)	//iterator must be int
		{	
			reg1 = getRegisterT();
			reg2 = getRegisterT();
			reg3 = getRegisterT();
				  //	reg2 $2	  reg1,reg2		reg3,$6.val.ival	reg2,reg3,reg1			$9.body					  label
		sprintf(str,"la %s,%s\nlw %s,0(%s)\nli $s,%d\nLOOP%s:\nnslt %s,%s,%s\nbnez $s,End%s\n$s\naddi $s,%s,%s\nj LOOP%s\nEnd%s:", reg2, (yyvsp[(2) - (9)].sval), reg1, reg2, reg3, (yyvsp[(6) - (9)].nVal).val.ival, label, reg2, reg3, reg1, reg2, label, (yyvsp[(9) - (9)].code).body, reg2, reg2, (yyvsp[(8) - (9)].code).body, label, label);
														//label		 			reg2,label			 reg2,reg2,$8.body		label
		(yyval.code).body = strConcat("codeBodyStr",str);
		(yyval.code).head = strConcat((yyvsp[(8) - (9)].code).head,(yyvsp[(9) - (9)].code).head);
		}
		else
		{	//error - iterator must be of type int !!!
			sprintf(errorMessage,"Iterator must be of type int");
			outputError(errorMessage);
			hasErrors = true;
			(yyval.code).body = strdup("");
			(yyval.code).head = strdup("");
		}

		freeRegisterT(reg1);
		freeRegisterT(reg2);
		freeRegisterT(reg3);
	}
	free(str);
	free(codeBodyStr);
}
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 1117 "cpm.y"
    { //1	   2	3		4  5  6   7		8	9 // @@@
	printf("CONTROL_STMT --> FOREACH ID ASSIGNOP NUM':'ID WITH STEP STMT\n");
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");

}
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 1124 "cpm.y"
    {
	outputError("expected an assignment operation");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 1131 "cpm.y"
    {
	outputError("expected ':'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 1138 "cpm.y"
    {
	outputError("expected ':'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 1145 "cpm.y"
    {//1	2		3	  4		5		6
/*do the block until BOOLEXPR is true*/
	printf("CONTROL_STMT --> DO STMT_BLOCK TILL '(' BOOLEXPR ')'\n");
	char* codeBodyStr = (char*)calloc((200 + strlen((yyvsp[(2) - (6)].code).body)),sizeof(char));
	char* label = getLabel();
	char* str = (char*)calloc((200 + strlen((yyvsp[(5) - (6)].decl).codeBody)),sizeof(char));

	//sprintf(codeBodyStr,"beq %s, $0, End%s\n%s\nj Loop%s\nEnd%s:\n", $5.reg, label, $2.body, label, label);
	sprintf(codeBodyStr,"bneq %s, $0, End%s\n%s\nj Loop%s\nEnd%s:\n", (yyvsp[(5) - (6)].decl).reg, label, (yyvsp[(2) - (6)].code).body, label, label);
	sprintf(str,"Loop%s:\n",label);
	strcat(str,(yyvsp[(5) - (6)].decl).codeBody);
	(yyval.code).head = strConcat((yyvsp[(5) - (6)].decl).codeHead,(yyvsp[(2) - (6)].code).head);
	(yyval.code).body = strConcat(str,codeBodyStr);
	freeRegisterT((yyvsp[(5) - (6)].decl).reg);
	free(str);
	free(codeBodyStr);
}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 1163 "cpm.y"
    {
	outputError("expected '('");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");	
}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 1170 "cpm.y"
    {
	outputError("Expected a boolean expression");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 1177 "cpm.y"
    {	
	outputError("expected ')'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");	
}
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 1184 "cpm.y"
    {//1
	printf("CONTROL_STMT --> switch\n");
	(yyval.code).head = strdup((yyvsp[(1) - (1)].decl).codeHead);
	(yyval.code).body = strdup((yyvsp[(1) - (1)].decl).codeBody);
}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 1192 "cpm.y"
    {//0		  1		2	   3
	printf("STMT_BLOCK --> '{' STMTLIST '}'\n");
	(yyval.code).head = strdup((yyvsp[(2) - (3)].code).head);
	(yyval.code).body = strdup((yyvsp[(2) - (3)].code).body);
}
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 1198 "cpm.y"
    { //error handling
	outputError("expected '{'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");	
}
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 1205 "cpm.y"
    {
	outputError("expected '}'");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");	
}
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 1214 "cpm.y"
    {//0		1	 2	  3		4	5	6	  7
	printf("switch --> SWITCH '(' CHOICE ')' '{' CASES '}'\n");
	(yyval.decl).codeBody = strConcat((yyvsp[(3) - (7)].decl).codeBody,(yyvsp[(6) - (7)].decl).codeBody);
	(yyval.decl).codeHead = strConcat((yyvsp[(3) - (7)].decl).codeHead,(yyvsp[(6) - (7)].decl).codeHead);
}
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 1220 "cpm.y"
    { 
	outputError("expected '('");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 1227 "cpm.y"
    { 
	outputError("expected a CHOICE");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 1234 "cpm.y"
    {
	outputError("expected ')'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 1241 "cpm.y"
    {
	outputError("expected '{'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 1248 "cpm.y"
    {
	outputError("expected '}'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 1257 "cpm.y"
    {//0	  1
	printf("CHOICE --> ID\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* reg;
	symTblEntry*  symbol = lookup(*ptr2symbolTable, (yyvsp[(1) - (1)].sval));

	if (symbol != NULL)
	{
		if(symbol->type == integer)
		{
			sprintf(codeBodyStr,"lw $s1,%s\n", (yyvsp[(1) - (1)].sval));
		}
		else
		{
			if(symbol->type == floating)
			{
				reg = getRegisterF();
				sprintf(codeBodyStr,"l.s %s,%s\n", reg, (yyvsp[(1) - (1)].sval));
				freeRegisterF(reg);
			}
		}
		(yyval.decl).codeBody = strdup(codeBodyStr);
	}
	else
	{
		outputError("Id not declared");
		hasErrors = true;
		(yyval.decl).codeBody = strdup("");
	}
	
	(yyval.decl).codeHead = strdup("");
	free(codeBodyStr);
}
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 1291 "cpm.y"
    {//1
	printf("CHOICE --> NUM\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* codeHeadStr = (char*)calloc(200,sizeof(char));
	char* reg;
	
	if((yyvsp[(1) - (1)].nVal).type == I)
	{
		sprintf(codeBodyStr, "li $s1,%d\n", (yyvsp[(1) - (1)].nVal).val.ival);
	}
	else
	{
		reg = getRegisterF();
		char* label = getLabel();
		sprintf(codeHeadStr,"%s: .float %f\n", label, (yyvsp[(1) - (1)].nVal).val.fval);
		sprintf(codeBodyStr,"l.s %s,%s\n", reg, label);
	}
	(yyval.decl).codeHead = strdup(codeHeadStr);
	(yyval.decl).codeBody = strdup(codeBodyStr);
	free(codeBodyStr);
	free(codeHeadStr);
}
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 1316 "cpm.y"
    {//0	 1	  2	  3		4		5	 6	  7
	printf("CASES --> CASE NUM ':' STMTLIST BREAK ';' CASES\n");
	char* codeBodyStr = (char*)calloc((200 + strlen((yyvsp[(4) - (7)].code).body)),sizeof(char));
	char* label = getLabel();
	char* reg =  getRegisterT();

	sprintf(codeBodyStr,"li %s, %d\nbne %s, $s1 ,next%s\n%s\nj end%s\nnext%s:\n", reg, (yyvsp[(2) - (7)].nVal).val.ival, reg, label, (yyvsp[(4) - (7)].code).body, (yyvsp[(7) - (7)].decl).label, label);	
	(yyval.decl).label = (yyvsp[(7) - (7)].decl).label;
	(yyval.decl).codeHead = strConcat((yyvsp[(4) - (7)].code).head, (yyvsp[(7) - (7)].decl).codeHead);
	(yyval.decl).codeBody = strConcat(codeBodyStr, (yyvsp[(7) - (7)].decl).codeBody);
	freeRegisterT(reg);
	free(codeBodyStr);
}
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 1330 "cpm.y"
    {
	outputError("expected ':'");
	hasErrors = true; 
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 1337 "cpm.y"
    {
	outputError("expected ';'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 1344 "cpm.y"
    {//1	   2	3
	printf("CASES --> DEFAULT ':' STMTLIST\n");
	char* codeBodyStr = (char*)calloc((200 + strlen((yyvsp[(3) - (3)].code).body)),sizeof(char));
	char* label = getLabel();

	sprintf(codeBodyStr,"default%s:\n%s\nEnd%s:", label, (yyvsp[(3) - (3)].code).body, label);
	(yyval.decl).label = strdup(label);
	(yyval.decl).codeHead = strdup((yyvsp[(3) - (3)].code).head);
	(yyval.decl).codeBody = strdup(codeBodyStr);
	free(codeBodyStr);
}
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 1356 "cpm.y"
    {
	outputError("expected ':'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 1365 "cpm.y"
    {//0	1	2		3	4	 5
	printf("STEP : ID ASSIGNOP ID ADDOP NUM\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char errorMessage[ERROR_STRING_LEN];
	symTblEntry*  symbol1 = lookup(*ptr2symbolTable, (yyvsp[(1) - (5)].sval));
	symTblEntry*  symbol2 = lookup(*ptr2symbolTable, (yyvsp[(3) - (5)].sval));

	if (symbol1 == NULL || symbol2 == NULL)
	{
		outputError("Id not declared");
		hasErrors = true;
		(yyval.code).head = strdup("");
		(yyval.code).body = strdup("");
		free(codeBodyStr);
	}
	else
	{
		if(symbol1->type == integer && symbol2->type == integer && (yyvsp[(5) - (5)].nVal).type == I) // all ints assignment
		{
			char* reg = getRegisterT();
			if((yyvsp[(4) - (5)].op) == PLUS)
				sprintf(codeBodyStr,"lw, %s, %s\naddi %s, %s ,%d\nsw %s, %s\n", reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.ival, reg, (yyvsp[(1) - (5)].sval));
			else
				sprintf(codeBodyStr,"lw, %s, %s\nsubi %s, %s ,%d\nsw %s, %s\n", reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.ival, reg, (yyvsp[(1) - (5)].sval));
			freeRegisterT(reg);
		}
		if(symbol1->type == floating && symbol2->type == floating && (yyvsp[(5) - (5)].nVal).type == F)	// all floats assignment
		{
			char* reg = getRegisterF();
			if((yyvsp[(4) - (5)].op) == PLUS)
			sprintf(codeBodyStr,"l.s, %s, %s\nadd.s %s, %s ,%d\ns.s %s, %s\n", reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.fval, reg, (yyvsp[(1) - (5)].sval));
			else
			sprintf(codeBodyStr,"l.s, %s, %s\nsub.s %s, %s ,%d\ns.s %s, %s\n", reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.fval, reg, (yyvsp[(1) - (5)].sval));
			freeRegisterF(reg);																	 
		}
	}
		
	(yyval.code).head = strdup("");
	(yyval.code).body = strdup(codeBodyStr);
	free(codeBodyStr);
}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 1407 "cpm.y"
    {
	outputError("expected an assigment operation");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 1414 "cpm.y"
    {
	outputError("expected an ID");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 1421 "cpm.y"
    {
	outputError("expected an operation");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 1428 "cpm.y"
    {
	outputError("expected a number");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 1435 "cpm.y"
    {//1	2	 3	  4	   5
	printf("STEP : ID ASSIGNOP ID MULOP NUM\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	symTblEntry*  symbol1 = lookup(*ptr2symbolTable, (yyvsp[(1) - (5)].sval));
	symTblEntry*  symbol2 = lookup(*ptr2symbolTable, (yyvsp[(3) - (5)].sval));

	if (symbol1 == NULL || symbol2 == NULL)
	{
		outputError("Id not declared");
		hasErrors = true;
		(yyval.code).head = strdup("");
		(yyval.code).body = strdup("");
		free(codeBodyStr);
	}
	else
	{
		if(symbol1->type == integer && symbol2->type == integer && (yyvsp[(5) - (5)].nVal).type == I)	// all ints assignment
		{
			char* reg = getRegisterT();
			if((yyvsp[(4) - (5)].op) == MUL)
				sprintf(codeBodyStr,"la ,%s, %s\nmul %s, %s ,%d\nsw %s, %s\n", reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.ival, reg, (yyvsp[(1) - (5)].sval));
			else
				sprintf(codeBodyStr,"la ,%s, %s\ndiv %s, %s ,%d\nsw %s, %s\n",reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.ival, reg, (yyvsp[(1) - (5)].sval));
			freeRegisterT(reg);
		}
		if(symbol1->type == floating && symbol2->type == floating && (yyvsp[(5) - (5)].nVal).type == F)	// all floats assignment
		{
			char* reg = getRegisterF();
			if((yyvsp[(4) - (5)].op) == MUL)
				sprintf(codeBodyStr,"l.s, %s, %s\nmul.s %s, %s ,%d\ns.s %s, %s\n", reg ,(yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.fval, reg, (yyvsp[(1) - (5)].sval));
			else
				sprintf(codeBodyStr,"l.s, %s, %s\ndiv.s %s, %s ,%d\ns.s %s, %s\n", reg, (yyvsp[(3) - (5)].sval), reg, reg, (yyvsp[(5) - (5)].nVal).val.fval, reg, (yyvsp[(1) - (5)].sval));
			freeRegisterF(reg);																	 
		}
	}
	(yyval.code).head = strdup("");
	(yyval.code).body = strdup(codeBodyStr);
	free(codeBodyStr);
}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 1475 "cpm.y"
    {
	outputError("expected an ID");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 1482 "cpm.y"
    {
	outputError("expected a number");
	hasErrors = true;
	(yyval.code).body = strdup("");
	(yyval.code).head = strdup("");
}
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 1491 "cpm.y"
    {//0		 1		 2		3
	printf("BOOLEXPR --> BOOLEXPR OROP BOOLTERM\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* tmp;
	char* reg = getRegisterT();
	char* label = getLabel();

	sprintf(codeBodyStr,"bne %s, %s, else%s\nadd %s, $0, %s\nj end%s\nelse%s: addi %s, $0, 1\nend%s", (yyvsp[(1) - (3)].decl).reg ,(yyvsp[(3) - (3)].decl).reg, label, reg, (yyvsp[(1) - (3)].decl).reg, label, label, reg, label);
	(yyval.decl).reg = strdup(reg);
	tmp = strConcat((yyvsp[(1) - (3)].decl).codeBody, (yyvsp[(3) - (3)].decl).codeBody);
	(yyval.decl).codeBody = strConcat(tmp, codeBodyStr);
	(yyval.decl).codeHead = strConcat((yyvsp[(1) - (3)].decl).codeHead, (yyvsp[(3) - (3)].decl).codeHead);
	freeRegisterT((yyvsp[(1) - (3)].decl).reg);
	freeRegisterT((yyvsp[(3) - (3)].decl).reg);
	free(tmp);
	free(codeBodyStr);
}
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 1509 "cpm.y"
    {//1
	printf("BOOLEXPR --> BOOLTERM\n");
	(yyval.decl).reg = strdup((yyvsp[(1) - (1)].decl).reg);
	(yyval.decl).codeBody = strdup((yyvsp[(1) - (1)].decl).codeBody);
	(yyval.decl).codeHead = strdup((yyvsp[(1) - (1)].decl).codeHead);
}
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 1516 "cpm.y"
    {
	outputError("expected an OROP");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 1525 "cpm.y"
    {//0		1		2		3
	printf("BOOLTERM --> BOOLTERM ANDOP BOOLFACTOR\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* tmp;
	char* reg = getRegisterT();
	
	sprintf(codeBodyStr,"mul %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg ,(yyvsp[(3) - (3)].decl).reg);
	(yyval.decl).reg = strdup(reg);
	tmp = strConcat((yyvsp[(1) - (3)].decl).codeBody, (yyvsp[(3) - (3)].decl).codeBody);
	(yyval.decl).codeBody = strConcat(tmp, codeBodyStr);
	(yyval.decl).codeHead = strConcat((yyvsp[(1) - (3)].decl).codeHead,(yyvsp[(3) - (3)].decl).codeHead);
	freeRegisterT((yyvsp[(1) - (3)].decl).reg);
	freeRegisterT((yyvsp[(3) - (3)].decl).reg);
	free(tmp);
	free(codeBodyStr);
}
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 1542 "cpm.y"
    {
	outputError("expected an ANDOP");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 1549 "cpm.y"
    {//1
	printf("BOOLTERM --> BOOLFACTOR\n");
	(yyval.decl).reg = strdup((yyvsp[(1) - (1)].decl).reg);
	(yyval.decl).codeBody = strdup((yyvsp[(1) - (1)].decl).codeBody);
	(yyval.decl).codeHead = strdup((yyvsp[(1) - (1)].decl).codeHead);
}
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 1558 "cpm.y"
    {//0		   1   2	3		  4
	/*Meaning not BOOLFACTOR*/
	printf("BOOLFACTOR -->  '!' '(' BOOLFACTOR ')'\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* reg = getRegisterT();
	
	sprintf(codeBodyStr,"li %s ,1\nsub %s, %s, %s\n", reg, (yyvsp[(3) - (4)].decl).reg, reg, (yyvsp[(3) - (4)].decl).reg);
	(yyval.decl).reg= (yyvsp[(3) - (4)].decl).reg;
	(yyval.decl).codeBody = strConcat((yyvsp[(3) - (4)].decl).codeBody, codeBodyStr);
	(yyval.decl).codeHead = (yyvsp[(3) - (4)].decl).codeHead;
	freeRegisterT(reg);
	free(codeBodyStr);
}
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 1572 "cpm.y"
    {
	outputError("expected an '('");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 1579 "cpm.y"
    {
	outputError("expected a BOOLFACTOR");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 1586 "cpm.y"
    {
	outputError("expected a ')'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 1593 "cpm.y"
    {//1			2		3
	printf("BOOLFACTOR -->  EXPRESSION  RELOP  EXPRESSION\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* tmpBody = (char*)calloc(200,sizeof(char));
	char* tmpHead = (char*)calloc(200,sizeof(char));
	char* reg = getRegisterT();

	if(strcmp((yyvsp[(1) - (3)].decl).type,"int") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"int") == 0) //RELOP between ints
	{
		if((yyvsp[(2) - (3)].op) == EQ)
			sprintf(codeBodyStr,"seq %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		if((yyvsp[(2) - (3)].op) == NEQ)
			sprintf(codeBodyStr,"sne %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		if((yyvsp[(2) - (3)].op) == LT)
			sprintf(codeBodyStr,"slt %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		if((yyvsp[(2) - (3)].op) == GT)
			sprintf(codeBodyStr,"sgt %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		if((yyvsp[(2) - (3)].op) == LTEQ)
			sprintf(codeBodyStr,"sle %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		if((yyvsp[(2) - (3)].op) == GTEQ)
			sprintf(codeBodyStr,"sge %s, %s, %s\n", reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);

		freeRegisterT((yyvsp[(1) - (3)].decl).reg);
		freeRegisterT((yyvsp[(3) - (3)].decl).reg);			
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"float") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"float") == 0) //RELOP between floats
	{
		char* label = getLabel();
		if((yyvsp[(2) - (3)].op) == EQ)
			sprintf(codeBodyStr,"c.eq.s %s, %s\nbc1f else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", (yyvsp[(1) - (3)].decl).reg ,(yyvsp[(3) - (3)].decl).reg, label, reg, label, label, reg, label);
		if((yyvsp[(2) - (3)].op) == NEQ)
			sprintf(codeBodyStr,"c.eq.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, label, reg, label, label, reg, label);
		if((yyvsp[(2) - (3)].op) == LT)
			sprintf(codeBodyStr,"c.lt.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, label, reg, label, label, reg, label);
		if((yyvsp[(2) - (3)].op) == GT)
			sprintf(codeBodyStr,"c.lt.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, label, reg, label, label, reg, label);
		if((yyvsp[(2) - (3)].op) == LTEQ)
			sprintf(codeBodyStr,"c.le.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, label, reg, label, label, reg, label);
		if((yyvsp[(2) - (3)].op) == GTEQ)
			sprintf(codeBodyStr,"c.le.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, label, reg, label, label, reg, label);

		freeRegisterF((yyvsp[(1) - (3)].decl).reg);
		freeRegisterF((yyvsp[(3) - (3)].decl).reg);
	}

	(yyval.decl).reg = strdup(reg);
	strcat(tmpBody,(yyvsp[(1) - (3)].decl).codeBody);
	strcat(tmpBody,(yyvsp[(3) - (3)].decl).codeBody);
	strcat(tmpBody,codeBodyStr);
	strcat(tmpHead,(yyvsp[(1) - (3)].decl).codeHead);
	strcat(tmpHead,(yyvsp[(3) - (3)].decl).codeHead);
	(yyval.decl).codeBody = strdup(tmpBody);
	(yyval.decl).codeHead = strdup(tmpHead);
	free(tmpBody);
	free(tmpHead);
	free(codeBodyStr);
}
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 1653 "cpm.y"
    {//0			1			2		3
	printf("EXPRESSION --> EXPRESSION  ADDOP  TERM\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* tmp;
	
	if(strcmp((yyvsp[(1) - (3)].decl).type,"int") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"int") == 0) //ADDOP between ints
	{
		if((yyvsp[(2) - (3)].op) == PLUS)
			sprintf(codeBodyStr,"add %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg);
		else
			sprintf(codeBodyStr,"sub %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		freeRegisterT((yyvsp[(1) - (3)].decl).reg);
		(yyval.decl).type = strdup("int");
		(yyval.decl).reg = strdup((yyvsp[(3) - (3)].decl).reg);
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"float") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"int") == 0)	//ADDOP between a float and an int
	{
		char* reg = getRegisterF();
		if((yyvsp[(2) - (3)].op) == PLUS)
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nadd.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, reg, reg, reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, reg);
		else
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nsub.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, reg, reg, reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, reg);
		freeRegisterF(reg);
		freeRegisterT((yyvsp[(3) - (3)].decl).reg);
		(yyval.decl).type = strdup("float");
		(yyval.decl).reg = strdup((yyvsp[(1) - (3)].decl).reg);
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"int") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"float") == 0)	//ADDOP between an int and a float
	{
		char* reg = getRegisterF();
		if((yyvsp[(2) - (3)].op) == PLUS)
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nadd.s %s, %s ,%s\n", (yyvsp[(1) - (3)].decl).reg, reg, reg, reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, reg);
		else
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nsub.s %s, %s ,%s\n", (yyvsp[(1) - (3)].decl).reg, reg, reg, reg, (yyvsp[(3) - (3)].decl).reg, reg, (yyvsp[(3) - (3)].decl).reg);
		freeRegisterF(reg);
		freeRegisterT((yyvsp[(1) - (3)].decl).reg);
		(yyval.decl).type = strdup("float");
		(yyval.decl).reg = strdup((yyvsp[(3) - (3)].decl).reg);
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"float") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"float") == 0)	//ADDOP between floats
	{
		if((yyvsp[(2) - (3)].op) == PLUS)
			sprintf(codeBodyStr,"add.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		else
			sprintf(codeBodyStr,"sub.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg);
		freeRegisterF((yyvsp[(1) - (3)].decl).reg);
		(yyval.decl).type = strdup("float");
		(yyval.decl).reg = strdup((yyvsp[(3) - (3)].decl).reg);
	}			
	
	tmp = strConcat((yyvsp[(1) - (3)].decl).codeBody, (yyvsp[(3) - (3)].decl).codeBody);
	(yyval.decl).codeBody= strConcat(tmp, codeBodyStr);
	(yyval.decl).codeHead= strConcat((yyvsp[(1) - (3)].decl).codeHead,(yyvsp[(3) - (3)].decl).codeHead);
	free(tmp);
	free(codeBodyStr);
}
    break;

  case 95:
/* Line 1792 of yacc.c  */
#line 1710 "cpm.y"
    {//1
	printf("EXPRESSION --> TERM\n");
	(yyval.decl).reg =  strdup((yyvsp[(1) - (1)].decl).reg);
	(yyval.decl).codeBody = strdup((yyvsp[(1) - (1)].decl).codeBody);
	(yyval.decl).codeHead = strdup((yyvsp[(1) - (1)].decl).codeHead);
	(yyval.decl).type = strdup((yyvsp[(1) - (1)].decl).type);
}
    break;

  case 96:
/* Line 1792 of yacc.c  */
#line 1720 "cpm.y"
    {//0	1	2		3
	printf("TERM --> TERM MULOP FACTOR\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* tmp;
	
	if(strcmp((yyvsp[(1) - (3)].decl).type,"int") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"int") == 0)	//MULOP between ints
	{
		if((yyvsp[(2) - (3)].op) == MUL) 
			sprintf(codeBodyStr,"mul %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg);
		else
			sprintf(codeBodyStr,"div %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg);
		freeRegisterT((yyvsp[(1) - (3)].decl).reg);
		(yyval.decl).type = strdup("int");
		(yyval.decl).reg = strdup((yyvsp[(3) - (3)].decl).reg);
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"float") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"int") == 0) //MULOP between a float and an int
	{
		char* reg = getRegisterF();
		if((yyvsp[(2) - (3)].op) == MUL) 
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nmul.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, reg, reg, reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, reg);
		else
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ndiv.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, reg, reg, reg, (yyvsp[(1) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg, reg);
		freeRegisterF(reg);
		freeRegisterT((yyvsp[(3) - (3)].decl).reg);
		(yyval.decl).type = strdup("float");
		(yyval.decl).reg = strdup((yyvsp[(1) - (3)].decl).reg);
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"int") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"float") == 0)	//MULOP between an int and a float
	{
		char* reg = getRegisterF();
		if((yyvsp[(2) - (3)].op) == MUL) 
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nmul.s %s, %s ,%s\n", (yyvsp[(1) - (3)].decl).reg, reg, reg, reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, reg);
		else
			sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ndiv.s %s, %s ,%s\n", (yyvsp[(1) - (3)].decl).reg, reg, reg, reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, reg);
		freeRegisterF(reg);
		freeRegisterT((yyvsp[(1) - (3)].decl).reg);
		(yyval.decl).type = strdup("float");
		(yyval.decl).reg = strdup((yyvsp[(3) - (3)].decl).reg);
	}
	if(strcmp((yyvsp[(1) - (3)].decl).type,"float") == 0 && strcmp((yyvsp[(3) - (3)].decl).type,"float") == 0)	//MULOP between floats
	{
		if((yyvsp[(2) - (3)].op) == MUL)
			sprintf(codeBodyStr,"mul.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg);
		else
			sprintf(codeBodyStr,"div.s %s, %s ,%s\n", (yyvsp[(3) - (3)].decl).reg, (yyvsp[(3) - (3)].decl).reg, (yyvsp[(1) - (3)].decl).reg);
		freeRegisterF((yyvsp[(1) - (3)].decl).reg);
		(yyval.decl).type = strdup("float");
		(yyval.decl).reg = strdup((yyvsp[(3) - (3)].decl).reg);
	}
	
	tmp = strConcat((yyvsp[(1) - (3)].decl).codeBody, (yyvsp[(3) - (3)].decl).codeBody);
	(yyval.decl).codeBody = strConcat(tmp, codeBodyStr);
	(yyval.decl).codeHead = strConcat((yyvsp[(1) - (3)].decl).codeHead, (yyvsp[(3) - (3)].decl).codeHead);
	free(tmp);
	free(codeBodyStr);
}
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 1777 "cpm.y"
    {
	outputError("expected a MULOP");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 1784 "cpm.y"
    {
	outputError("expected a FACTOR");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 1791 "cpm.y"
    {//1
	printf("TERM --> FACTOR\n");
	(yyval.decl).reg =  strdup((yyvsp[(1) - (1)].decl).reg);
	(yyval.decl).codeBody = strdup((yyvsp[(1) - (1)].decl).codeBody);
	(yyval.decl).codeHead = strdup((yyvsp[(1) - (1)].decl).codeHead);
	(yyval.decl).type = strdup((yyvsp[(1) - (1)].decl).type);
}
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 1801 "cpm.y"
    {//0	  1		2		3
	printf("FACTOR --> '(' EXPRESSION ')'\n");
	(yyval.decl).reg =  strdup((yyvsp[(2) - (3)].decl).reg);
	(yyval.decl).codeBody = strdup((yyvsp[(2) - (3)].decl).codeBody);
	(yyval.decl).codeHead = strdup((yyvsp[(2) - (3)].decl).codeHead);
	(yyval.decl).type = strdup((yyvsp[(2) - (3)].decl).type);
}
    break;

  case 101:
/* Line 1792 of yacc.c  */
#line 1809 "cpm.y"
    {
	outputError("expected a ')'");
	hasErrors = true;
	(yyval.decl).codeBody = strdup("");
	(yyval.decl).codeHead = strdup("");
}
    break;

  case 102:
/* Line 1792 of yacc.c  */
#line 1816 "cpm.y"
    {//1
	printf("FACTOR --> ID\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* reg;
	char* label = getLabel();
	symTblEntry* symbol = lookup(*ptr2symbolTable, (yyvsp[(1) - (1)].sval));
	
	if(symbol != NULL)
	{
		if(symbol->type == integer)
		{
			reg = getRegisterT();
			sprintf(codeBodyStr,"lw %s, %s\n", reg, (yyvsp[(1) - (1)].sval));
		}
		if(symbol->type == floating)
		{
			reg = getRegisterF();
			sprintf(codeBodyStr,"l.s %s, %s\n", reg, (yyvsp[(1) - (1)].sval));
		}
		if(symbol->type == string)
		{
			reg = (yyvsp[(1) - (1)].sval);
		}
		(yyval.decl).reg = strdup(reg);
		(yyval.decl).type = EnumType2charType(symbol->type);
		(yyval.decl).codeBody = strdup(codeBodyStr);
		(yyval.decl).codeHead = strdup("");
	}
	else
	{
		outputError("Id not declared");
		hasErrors = true;
		(yyval.decl).codeBody = strdup("");
		(yyval.decl).codeHead = strdup("");
	}
	free(codeBodyStr);
}
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 1854 "cpm.y"
    {//1
	printf("FACTOR --> NUM\n");
	char* codeBodyStr = (char*)calloc(200,sizeof(char));
	char* codeHeadStr = (char*)calloc(200,sizeof(char));
	char* reg;
	
	if((yyvsp[(1) - (1)].nVal).type == I) //int
	{
		reg = getRegisterT();
		sprintf(codeBodyStr,"addi %s, $0, %d\n", reg, (yyvsp[(1) - (1)].nVal).val.ival);
		(yyval.decl).type = strdup("int");

	}
	else	//$1.type == F
	{
		reg = getRegisterF();
		char* label = getLabel();
		sprintf(codeHeadStr,"%s: .float %f\n", label, (yyvsp[(1) - (1)].nVal).val.fval);
		sprintf(codeBodyStr,"l.s %s, %s\n", reg, label);
		(yyval.decl).type = strdup("float");
	}
	(yyval.decl).reg = strdup(reg);
	(yyval.decl).codeHead = strdup(codeHeadStr);
	(yyval.decl).codeBody = strdup(codeBodyStr);
	free(codeHeadStr);
	free(codeBodyStr);
}
    break;


/* Line 1792 of yacc.c  */
#line 3992 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 1881 "cpm.y"


int main (int argc, char **argv)
{  
	extern FILE* yyin;
	FILE* listFile;
	FILE* mipsFile;
	char line[500];
	char* inputFileName;
	char* inputFileType;
	int linesCounter = 1;
	symTbl* symbolTable = NULL;

	printf("%s", STUDENTS_DETAILS);
	printf("Starting compilation process...\n");
	
	symbolTable = initSymTbl();
	if (symbolTable == NULL)
	{
		fprintf(stderr, "Symbol table initialization failed. Operation terminated\n");
		exit(1);
	}
	ptr2symbolTable = &symbolTable;
	
// ########  input validation  ########
	printf("Input validation stage started\n");
	if (argc >= 2 && argv[1] != NULL)
	{
		inputFileName = strdup(argv[1]);
		inputFileType = strrchr(inputFileName, '.');
		if (strcmp(inputFileType, ".cpl") != 0 && strcmp(inputFileType, ".CPL") != 0)
		{
			fprintf(stderr, "%s files are not supported by this compiler. Operation terminated\n", inputFileType);
			free(inputFileName);
			free(inputFileType);
			destroySymTable(*ptr2symbolTable);
			exit(1);
		}
		free(inputFileName);
		free(inputFileType);
		yyin = fopen(argv[1], "r");
		
		if (yyin == NULL)
		{
			fprintf(stderr, "Failed to open input file. Operation terminated!\n");
			destroySymTable(*ptr2symbolTable);
			exit(1);
		}

	}
	else
	{
		fprintf(stderr, "No input file given to compiler. Operation terminated!\n");
		destroySymTable(*ptr2symbolTable);
		exit(1);
	}
	printf("Input validation stage completed\n");

// ########  End of input validation  ########
	
// ########  copy to list file section  ########
	

	printf("Creating listing file\n");
	listFile = fopen("listing.lst","w");
	if (listFile == NULL)
	{
		fprintf(stderr, "Failed to open list file. Operation terminated!\n");
		destroySymTable(*ptr2symbolTable);
		fclose(yyin);
		exit(1);
	}

	fprintf(listFile, STUDENTS_DETAILS);
	printf("Copying input code to listing file\n");
	do{
		fgets(line, 500, yyin);
		fprintf(listFile, "%d: %s", linesCounter++, line);
	}while(!feof(yyin));
	fprintf(listFile, "\n");
	printf("Copying input code to listing file completed\n");
	fclose(listFile);

// ########  End of copy to list file section  ########

	fseek(yyin, 0, SEEK_SET); //moving cursor to the beginning for lexical analysis
	printf("Starting parsing process\n");
	yyparse();
	printf("Parsing process completed\n");

	if (hasErrors == false) //no errors were found during parsing process -> creating MIPS file
	{
		if (strlen(mipsCode) == 0)
			fprintf(stderr, "Unexpected error occurred. MIPS file can not be created\n");
		else
		{
			printf("Creating MIPS file\n");
			mipsFile = fopen("MIPS.asm","w+");
			if (mipsFile == NULL)
			{
				fprintf(stderr, "Failed to open mips file. Operation terminated!\n");
				destroySymTable(*ptr2symbolTable);
				fclose(yyin);
				exit(1);
			}
			printf("MIPS file has been created\n");
			fprintf(mipsFile, "#");
			fprintf(mipsFile, STUDENTS_DETAILS);
			fprintf(mipsFile, mipsCode);
			fclose (mipsFile);
		}

	}
	else
		fprintf(stderr, "Errors were found. See liting file for details\n");
	
	fclose (yyin);
	printf("Destroying symbol table\n");
	printf("Compilation process is done\n");
	destroySymTable(*ptr2symbolTable);
	return 0;
}

void yyerror (char *s)
{}
