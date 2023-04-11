%{
#define _CRT_SECURE_NO_WARNINGS
#define alloca _alloc
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR_STRING_LEN 100
#define STUDENTS_DETAILS "Vadim Darchuk 316920974 and Yotam Alter 302955679\n\n"
#define SYM_TBL_ROWS_COUNT 20
// #define DEBUG_PRINT

extern int yylex();
extern int yylineno;
void yyerror (char *s);

// ############### Symbol table definitions ####################
typedef enum Type 
{
   integer, 
   real, 
   string
}Type;

typedef struct symTblEntry
{
   char* name;
   bool isConst;
   bool isDeclared;
   bool isInit;
   Type type;
   int occurences;
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

// ############### debug print function #############################
/*
* Debug print function
*/
#ifdef DEBUG_PRINT
void dbg_print(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "DEBUG: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}
#else
void dbg_print(const char* format, ...) {}
#endif

/*
* String duplicate function
*/
char* string_duplicate(char* orig_str)
{
   char* new_str = (char*)malloc(strlen(orig_str)+1);
   strcpy(new_str, orig_str);
   return new_str;
}

// ############### Symbol table related functions ####################
/*
* Initiates a symbol table for the compilation process
*/
symTbl* initSymTbl()
{
    symTbl* symbolTbl = (symTbl*)calloc(1, sizeof(symTbl));

    if (symbolTbl == NULL)
    {
        dbg_print("Failed to init a symbol table. Compilation process is terminated");
        fprintf(stderr, "Failed to init a symbol table. Compilation process is terminated\n");
        exit(1);
    }

    return symbolTbl;
}

/*
* Calculates a hash value for a given lexeme
*/
int getHash(char* lexeme)
{
   int len_lex = strlen(lexeme);
   if (lexeme == NULL || len_lex < 1)
   {
      return -1;
   }
   return len_lex % SYM_TBL_ROWS_COUNT;
}


// Searches for an entry in a symbol table using an efficient hash-based algorithm.
symTblEntry* lookupSymbol(symTbl* symTbl, char* lexeme)
{
    if (symTbl == NULL || lexeme == NULL)
    {
        dbg_print("symTbl == NULL || lexeme == NULL");
        return NULL;
    }

    int hashValue = getHash(lexeme);

    if (hashValue == -1)
    {
        dbg_print("Failed to get hash value");
        fprintf(stderr, "Failed to get hash value\n");
        return NULL;
    }

    dbg_print("looking for: %s, hashValue: %d", lexeme, hashValue);

    symTblEntry* ptr = symTbl->symbolTableHead[hashValue];

    while (ptr != NULL)
    {
        dbg_print("ptr not null: %s", ptr->name);

        if (strcmp(ptr->name, lexeme) == 0)
        {
            dbg_print("found symbol: %s", lexeme);
            return ptr;
        }
        
        ptr = ptr->next;
    }

    dbg_print("symbol not in table: %s", lexeme);
    return NULL;
}


// Creates a new symbol instance.
symTblEntry* createSymTblEntry(symTbl* symTbl, char* lexeme, Type type, bool isConst, int hashValue)
{
    if (symTbl == NULL || lexeme == NULL || hashValue < 0 || hashValue >= SYM_TBL_ROWS_COUNT)
    {
        dbg_print("Bad arguments passed to createSymTblEntry");
        fprintf(stderr, "Bad arguments passed to createSymTblEntry\n");
        return NULL;
    }

    symTblEntry* newSymTblEntry = (symTblEntry*)calloc(1, sizeof(symTblEntry));
    if (newSymTblEntry == NULL)
    {
        dbg_print("Failed to allocate memory for a new symbol");
        fprintf(stderr, "Failed to allocate memory for a new symbol\n");
        return NULL;
    }

    newSymTblEntry->name = string_duplicate(lexeme);
    newSymTblEntry->isConst = isConst;
    newSymTblEntry->isDeclared = true;
    newSymTblEntry->isInit = false;
    newSymTblEntry->type = type;
    newSymTblEntry->occurences = 1;
    newSymTblEntry->next = NULL;

    return newSymTblEntry;
}


// Adds a symbol to the symbol table.
// The added symbol is added to the tail of the appropriate line of the symbol table.
void insertEntry(symTbl* symTbl, symTblEntry* newEntry, int hashValue)
{
    if (symTbl == NULL || newEntry == NULL || hashValue < 0 || hashValue >= SYM_TBL_ROWS_COUNT)
    {
        dbg_print("Bad arguments passed to insertEntry");
        fprintf(stderr, "Bad arguments passed to insertEntry\n");
        return;
    }

    if (symTbl->symbolTableHead[hashValue] == NULL) // the symbol table row is empty
    {
        dbg_print("symbol is null, inserting new symbol");
        symTbl->symbolTableHead[hashValue] = newEntry;
    }
    else // otherwise, add newEntry to the tail of the appropriate list
    {
        dbg_print("add newEntry to the tail of the appropriate list");
        symTblEntry* temp = symTbl->symbolTableHead[hashValue];
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = newEntry;
    }
}


// Converts char* type to Enum Type of symbol table.
Type charType2EnumType(char* type)
{
    if (type == NULL) // for argument protection only
    {
        return integer;
    }
    else if (strcmp(type, "int") == 0)
    {
        return integer;
    }
    else if (strcmp(type, "real") == 0)
    {
        return real;
    }
    else if (strcmp(type, "string") == 0)
    {
        return string;
    }
    
    return integer; // Default to integer for unknown types.
}


// Converts Enum Type of symbol table to char* type.
char* EnumType2charType(Type type)
{
    switch (type)
    {
        case integer:
            return string_duplicate("int");
        case real:
            return string_duplicate("float");
        case string:
            return string_duplicate("string");
        default:
            return NULL; // Return NULL for unknown types.
    }
}

// A wrapper function for adding a new symbol to a symbol table.
void addSymbol(symTbl* symTbl, char* lexeme, char* type, bool isConst)
{
    if (symTbl == NULL || lexeme == NULL || type == NULL)
    {
        dbg_print("Bad arguments passed to addSymbol");
        fprintf(stderr, "Bad arguments passed to addSymbol\n");
        return;
    }

    int hashValue = getHash(lexeme);
    if (hashValue == -1)
    {
        dbg_print("Bad hash value returned");
        fprintf(stderr, "Bad hash value returned\n");
        return;
    }

    if (lookupSymbol(symTbl, lexeme) != NULL) // entry already in symbol table
    {
        dbg_print("Symbol already in symbol table");
        fprintf(stderr, "%s is already in symbol table\n", lexeme);
        return;
    }

    dbg_print(type);
    dbg_print(lexeme);
    Type newType = charType2EnumType(type);
    symTblEntry* newSymbol = createSymTblEntry(symTbl, lexeme, newType, isConst, hashValue);

    if (newSymbol == NULL)
    {
        dbg_print("Null Symbol");
        return;
    }
    insertEntry(symTbl, newSymbol, hashValue);
}

// This function doesn't handle the linked-list connections. It's the caller's responsibility.
void destroyEntry(symTblEntry* entry)
{
    if (entry == NULL)
    {
        dbg_print("Entry to be destroyed is NULL");
        fprintf(stderr, "Entry to be destroyed is NULL!!\n");
        return;
    }
    if (entry->type == string)
    {
        free(entry->value.sval);
    }
    free(entry->name);
    free(entry);
}

// This function is responsible for destroying the entire symbol table at the end of the compilation process.
void destroySymTable(symTbl* symTbl)
{
    int i;
    symTblEntry* symbol = NULL;
    symTblEntry* nextSymbol = NULL;
    if (symTbl == NULL)
    {
        return;
    }

    for (i = 0; i < SYM_TBL_ROWS_COUNT; i++)
    {
        symbol = symTbl->symbolTableHead[i];
        while (symbol != NULL)
        {
            nextSymbol = symbol->next;
            destroyEntry(symbol);
            symbol = nextSymbol;
        }
    }
    free(symTbl);
}


// ############### End of symbol table related functions ####################

// ############### Global variables ####################

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
void output_error(char* s){
     FILE* listFile;
     listFile = fopen("listing.lst","a+");
     hasErrors = true;
     if (listFile == NULL)
     {
       return;
     }
     else
     {
       fprintf(listFile,"ERROR in line: %d, %s\n", yylineno, s);
       fclose(listFile);
     }
}


/*
* Concatenates two strings into one - useful for appending mips code as it is gets written
*/
char* string_concat(char* str1, char* str2)
{
   char* newString =(char*)calloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1), sizeof(char));
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
   return string_duplicate(str);
}


// ############### End of service functions ###################
// ############### BISON related definitions ##################
%}
%locations
%union
{
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

   struct declaration
   {
      int ival;
      float fval;
      char *idval;
      char* type;
      char *IDarray[5];
      char *reg;
      char *label;
      char *codeHead;
      char *codeBody;
   }decl;

   struct mipsCode
   {
      char *label;
      char *head;
      char *body;
   }code;
}

%token <nVal> NUM
%token <op> ADDOP MULOP ASSIGNOP OROP ANDOP RELOP
%token <sval> ID STRING_LITERAL 
%token START BREAK CASE FINAL DCL DEFAULT ELSE END FOREACH IF IN INT OUT PROGRAM REAL STRING SWITCH THEN TILL WHILE WITH

%type <decl> EXPRESSION TERM FACTOR TYPE LIST DECLARATIONS DECLARLIST DECL CDECL switch CHOICE CASES BOOLEXPR BOOLTERM BOOLFACTOR
%type <code> STMTLIST STMT ASSIGNMENT_STMT CONTROL_STMT STMT_BLOCK OUT_STMT IN_STMT STEP
%start program

%left addop
%left mulop
%left orop
%left andop
%left relop
%right assignop
%%
program : PROGRAM ID START DECLARATIONS STMTLIST END
{// 0      1       2   3         4         5      6
   printf("PROGRAM --> PROGRAM ID START DECLARATIONS STMTLIST END\n");
   char* str = (char*)calloc(1000, sizeof(char));
   
   if($5.head == NULL) $5.head = "";
   if($4.codeBody == NULL) $4.codeBody = "";
   sprintf(str,".data\n%s\n%s\nstrBuff: .space 200\n.text\nProgram:\n%s\n%s\n", $4.codeHead, $5.head, $4.codeBody, $5.body);
   dbg_print(str);
   mipsCode = str;
}


DECLARATIONS : DCL DECLARLIST CDECL
{//0            1      2      3
   printf("DECLARATIONS --> DCL DECLARLIST CDECL\n");
   $$.codeHead = string_duplicate(strcat($2.codeHead, $3.codeHead));
   $$.codeBody = string_duplicate($3.codeBody);
   dbg_print($$.codeHead);
   dbg_print($$.codeBody);
}
|
{
   printf("DECLARATIONS --> epsilon\n");
   $$.codeHead = string_duplicate("");
   $$.codeBody = string_duplicate("");
};


DECLARLIST : DECLARLIST DECL
{
   printf("DECLARLIST --> DECLARLIST DECL\n");
   $$.codeHead = string_concat($1.codeHead, $2.codeHead);
}
|DECL
{
   printf("DECLARLIST --> DECL\n");
   $$.codeHead = string_duplicate($1.codeHead);
};


DECL :  TYPE':'LIST
{//0     1   2   3
   printf("DECL -->  TYPE ':' LIST\n");
   char codeHeadStr[200] = { 0 };
   char errorMessage[ERROR_STRING_LEN]; 
   int i = 0;
   
   do
   {
      dbg_print("Checking array member:");
      dbg_print($3.IDarray[i]);
      if(lookupSymbol(*ptr2symbolTable, $3.IDarray[i]) == NULL)
         {
            dbg_print("Symbol type:");
            dbg_print($1.type);
            addSymbol(*ptr2symbolTable, $3.IDarray[i], $1.type, false);
            if(strcmp($1.type,"string") == 0)
            {
               sprintf(codeHeadStr,"%s: .space 200\n",$3.IDarray[i]);
            }
            else
            {
               sprintf(codeHeadStr,"%s: .space 8\n",$3.IDarray[i]);
            }
         }
         else 
         {   
            sprintf(errorMessage, "Duplicated declaration. %s already declared\n", $3.IDarray[i]);
            output_error(errorMessage);
            sprintf(codeHeadStr,"");
         }
      i++;
   } while ($3.IDarray[i] != NULL);

   $$.codeHead = string_duplicate(codeHeadStr);
}
|TYPE LIST 
{
   output_error("Expected ':' ");
   $$.codeHead = string_duplicate("");
};


LIST : ID','LIST
{//0   1  2   3
   printf("LIST --> ID ',' LIST\n");
   int i, j;
   for(i = 0 ; i < 5 ; i++)   //Assuming up to 5 Ids in a chained declaration
   {
      if($$.IDarray[i] == NULL)
      {
         $$.IDarray[i] = $1;
            for(j = 0 ; j < 5 ; j++)
            {
               if($3.IDarray[i] != NULL)
               {
                  $$.IDarray[i+1] = $3.IDarray[j];
                  i++;
               }
            }
         break;
      }
   }
}

|ID';'
{//1
   printf("LIST --> ID ';'\n");
   int i;
   for(i = 0 ; i < 5 ; i++)   //Assuming up to 5 Ids in a chained declaration
   {
      if($$.IDarray[i] == NULL)
      {
         $$.IDarray[i] = $1;
         break;
      }
   }
}
|ID 
{ 
   output_error("Expected ';' "); 
}
|ID LIST
{
   output_error("Expected ',' ");
};


TYPE : INT
{//0    1
   printf("TYPE --> INT\n");
   $$.type = string_duplicate("int");
}
|REAL
{//1
   printf("TYPE --> REAL\n");
   $$.type = string_duplicate("float");
}
|STRING
{//1
   printf("TYPE --> STRING\n");
   $$.type = string_duplicate("string");
};


CDECL : FINAL TYPE ID ASSIGNOP NUM';'CDECL
{//0     1      2   3   4       5  6   7
   printf("CDECL --> FINAL TYPE ID ASSIGNOP NUM ';' CDECL\n");
   char codeBodyStr[200] = { 0 };
   char codeHeadStr[200] = { 0 };
   char errorMessage[ERROR_STRING_LEN];
   if(lookupSymbol(*ptr2symbolTable, $3) == NULL) //id not in symbol table
   {
      addSymbol(*ptr2symbolTable, $3, $2.type, true);
      if(strcmp($2.type,"string")==0)
         sprintf(codeHeadStr,"%s: .space 200\n",$3);
      else
      {
         sprintf(codeHeadStr,"%s: .space 8\n",$3);
         if(strcmp($2.type,"int")==0)
            sprintf(codeBodyStr,"li $t0,%d\nsw $t0, %s\n",$5.val,$3);
      }
   }
   else
   {
      sprintf(errorMessage, "Duplicated declaration. %s already declared\n", $3);
      output_error(errorMessage);
      sprintf(codeHeadStr,"");

   }
   $$.codeHead = string_concat(codeHeadStr, $7.codeHead);
   $$.codeBody = string_concat(codeBodyStr, $7.codeBody);
}
|FINAL TYPE ID ASSIGNOP NUM CDECL
{
   output_error("Expected ';' ");

   $$.codeHead = string_duplicate("");
   $$.codeBody = string_duplicate("");
}
| 
{
   printf("CDECL --> epsilon\n");
   $$.codeHead = string_duplicate("");
   $$.codeBody = string_duplicate("");
};


STMTLIST : STMTLIST STMT
{//0      1       2
   printf("STMTLIST --> STMTLIST STMT\n");
   $$.body = string_concat($1.body, $2.body);
}
|
{
   printf("STMTLIST --> epsilon\n");
   $$.body = string_duplicate("");
};


STMT : ASSIGNMENT_STMT
{//0   1
   printf("STMT --> ASSIGNMENT_STMT\n");
   $$.head = string_duplicate($1.head);
   $$.body = string_duplicate($1.body);
}
|CONTROL_STMT
{   //1
   printf("STMT --> CONTROL_STMT\n");
   $$.head = string_duplicate($1.head);
   $$.body = string_duplicate($1.body);
}
|IN_STMT
{ //1
   printf("STMT --> IN_STMT\n");
   $$.body = string_duplicate($1.body);
}
|OUT_STMT
{ //1 
   printf("STMT --> OUT_STMT\n");
   $$.head = string_duplicate($1.head);
   $$.body = string_duplicate($1.body);
}
|STMT_BLOCK
{ //1
   printf("STMT --> STMT_BLOCK\n");
   $$.head = string_duplicate($1.head);
   $$.body = string_duplicate($1.body);
};


OUT_STMT : OUT'('EXPRESSION')'';'
{//0        1  2   3        4  5
   printf("OUT_STMT --> OUT '(' EXPRESSION ')' ';'\n");
   char codeBodyStr[200] = { 0 };
   dbg_print("out(d)");
   dbg_print($3.type);
   if(strcmp($3.type, "int") == 0)   //print an int
   {
      sprintf(codeBodyStr, "li $v0, 1\nlw $a0, %s\nsyscall\n", $3.reg);
      dbg_print("print int");
   }
   else if(strcmp($3.type, "real") == 0)   //print a float
   {
      sprintf(codeBodyStr,"li $v0, 2\nl.s $f12,%s\n syscall\n",$3.reg);
      dbg_print("print float");
   }
   else if(strcmp($3.type, "string") == 0)  //print a string
   {
      dbg_print("print string");
      sprintf(codeBodyStr,"li $v0,4\nlw $a0, %s\nsyscall\n", $3.reg);
   }
   else
   {
      dbg_print("wrong type");
      output_error("Unknown type");

      $$.body = string_duplicate("");
      $$.head = string_duplicate("");
   }
   
   dbg_print(codeBodyStr);
   $$.head = $3.codeHead;
   dbg_print($$.head);
   $$.body = string_concat($3.codeBody, codeBodyStr);
   dbg_print($$.body);
}
|OUT EXPRESSION')'';'
{
   output_error("Expected '(' ");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|OUT'('EXPRESSION ';'
{
   output_error("Expected ')'");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|OUT'('EXPRESSION')'
{ 
   output_error("expected ';' ");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
};


OUT_STMT: OUT'('STRING_LITERAL')'';'
{//0        1 2   3            4  5
   printf("OUT_STMT --> OUT '(' STRING_LITERAL ')' ';'\n");
   char codeBodyStr[200] = { 0 };
   char codeHeadStr[200] = { 0 };
   char* label = getLabel();
   
   sprintf(codeHeadStr, "%s: .asciiz %s\n", label, $3);
   sprintf(codeBodyStr, "la $a0,%s\nli $v0,4\nsyscall\n", label); //print a string
   
   $$.head = string_duplicate(codeHeadStr);
   $$.body = string_duplicate(codeBodyStr);
}   // error handling
|OUT STRING_LITERAL')'';'
{
   output_error("Expected '('");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|OUT'('STRING_LITERAL ';'
{
   output_error("Expected ')'");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|OUT'('STRING_LITERAL')'
{
   output_error("expected ';'");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
};


IN_STMT : IN'('ID')'';'
{//0      1     2     3    4    5
   printf("IN_STMT --> IN '(' ID ')' ';'\n");
   char codeBodyStr[200] = { 0 };
   char errorMessage[ERROR_STRING_LEN];

   symTblEntry*  symbol = lookupSymbol(*ptr2symbolTable, $3);
   if (symbol != NULL)
   {
      if(symbol->isConst == false)
      {
         if(symbol->type == integer)   //read int
            sprintf(codeBodyStr,"li $v0,5\nsyscall\nsw $v0, %s\n", symbol->name);

         if(symbol->type == real) //read float
            sprintf(codeBodyStr,"li $v0,6\nsyscall\ns.s $f0, %s\n", symbol->name);

         if(symbol->type == string)   //read string
         {
            strcat(codeBodyStr,"li $v0,8\nla $a0,");
            strcat(codeBodyStr,symbol->name);
            strcat(codeBodyStr,"\nli $a1,200\nsyscall\n");
         }
      }
      else
      {
         sprintf(errorMessage,"Can't assign to a constant");
         output_error(errorMessage);
         sprintf(codeBodyStr,"");
   
      }
   }
   else
   {
      sprintf(errorMessage,"Id isn't declared");
      output_error(errorMessage);
      sprintf(codeBodyStr,"");

   }
   $$.body = string_duplicate(codeBodyStr);
}
|IN ID')'';'
{
   output_error("expected '('");

   $$.body=string_duplicate("");
}
|IN'('ID ';'
{
   output_error("expected ')'");

   $$.body=string_duplicate("");
}
|IN'('ID')'
{
   output_error("expected ';'");

   $$.body=string_duplicate("");
};


ASSIGNMENT_STMT : ID ASSIGNOP EXPRESSION';'
{//0               1      2      3       4
   printf("ASSIGNMENT_STMT --> ID ASSIGNOP EXPRESSION';'\n");
   char codeBodyStr[200] = { 0 };
   char errorMessage[ERROR_STRING_LEN];
   char* reg;

   symTblEntry*  symbol = lookupSymbol(*ptr2symbolTable, $1);
   if (symbol != NULL)
   {
      if(symbol->isConst == false)
      { 
         if(strcmp($3.type,"int") == 0 && symbol->type == integer) //ints assignment
         {
            sprintf(codeBodyStr,"sw %s, %s\n", $3.reg, $1);
            freeRegisterT($3.reg);
         }   
         else if(strcmp($3.type,"float") == 0 && symbol->type == real) //floats assignment
         {
            sprintf(codeBodyStr,"s.s %s, %s\n", $3.reg, $1);
            freeRegisterF($3.reg);
         }
            else
            {
               if(strcmp($3.type,"int")==0 && symbol->type == real) //assign an int into a float
               {
                  reg = getRegisterF();
                  sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ns.s %s, %s\n", $3.reg, reg, reg, reg, reg, $1);
                  freeRegisterT($3.reg);
                  freeRegisterF(reg);
               }
               else   // attempting to assign a float into an int
               {
                  sprintf(errorMessage,"Can't convert from a real number to integer. Illegal assignment");
                  output_error(errorMessage);
                  sprintf(codeBodyStr,"");
            
               }
            }
      }
      else
      {
         sprintf(errorMessage,"Can't assign to a constant");
         output_error(errorMessage);
         sprintf(codeBodyStr,"");
   
      }
   }
   else
   {
      sprintf(errorMessage,"Id isn't declared");
      output_error(errorMessage);
      sprintf(codeBodyStr,"");

   }

   $$.head = string_duplicate($3.codeHead);
   $$.body = string_concat($3.codeBody, codeBodyStr);
}
|ID ASSIGNOP EXPRESSION
{
   output_error("expected ';'");

   $$.head = string_duplicate("");
   $$.body = string_duplicate("");
}
|ID ASSIGNOP
{
   output_error("Expected assignment value");

   $$.head = string_duplicate("");
   $$.body = string_duplicate("");
 };


ASSIGNMENT_STMT : ID ASSIGNOP STRING_LITERAL';'
{// 0              1    2            3       4
   printf("STMT --> ID ASSIGNOP STRING_LITERAL ';'\n");
   
   char codeBodyStr[200] = { 0 };
   char codeHeadStr[200] = { 0 };
   char errorMessage[ERROR_STRING_LEN];

   char* reg = getRegisterT();
   char* label = getLabel();
   symTblEntry*  symbol = lookupSymbol(*ptr2symbolTable, $1);
   dbg_print("Symbol name:");
   dbg_print(symbol->name);
   if (symbol != NULL)
   {   
      if(symbol->isConst == false)
      {
         if(symbol->type == string)
         {
	    dbg_print("Symbol name:");
	    dbg_print(symbol->name);
            free(symbol->name);
            symbol->value.sval = string_duplicate($3);
            symbol->name = string_duplicate($1);
	    dbg_print("Symbol name:");
	    dbg_print(symbol->name);
	    dbg_print("Symbol value");
	    dbg_print(symbol->value.sval);
            sprintf(codeHeadStr,"%s: .asciiz %s\n", label, $3);
            sprintf(codeBodyStr,"lw %s, %s\nsw %s, %s\n", reg, label, reg, $1);
            dbg_print(codeHeadStr);
            dbg_print(codeBodyStr);
         }
         else
         {
            sprintf(errorMessage, "Trying to assign sentence to an id which is not of type string");
            output_error(errorMessage);
            sprintf(codeBodyStr,"");
      
         }
      }
      else
      {
         sprintf(errorMessage, "Can not assign to a constant variable");
         output_error(errorMessage);
         sprintf(codeBodyStr,"");
   
         }
   }
   else
   {
      sprintf(errorMessage,"string id is not declared");
      output_error(errorMessage);
      sprintf(codeBodyStr,"");

   }

   freeRegisterT(reg);
   $$.head = string_duplicate(codeHeadStr);
   $$.body = string_duplicate(codeBodyStr);
}
|ID ASSIGNOP STRING_LITERAL
{ 
   output_error("Expected ';' ");

   $$.head = string_duplicate("");
   $$.body = string_duplicate("");
};


CONTROL_STMT : IF'('BOOLEXPR')'THEN STMT ELSE STMT
{//0            1 2   3      4    5    6    7   8
   printf("CONTROL_STMT --> IF '(' BOOLEXPR ')'THEN  STMT ELSE STMT\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($6.body)+ strlen($8.body)),sizeof(char));
   char* label = getLabel();
   char* str;
   
   sprintf(codeBodyStr,"beq %s,$0,Else%s\n%s\nj End%s\nElse%s:\n%s\nEnd%s:\n", $3.reg, label, $6.body, label, label ,$8.body, label);   
   str = string_concat($6.head, $8.head);
   $$.head = string_concat($3.codeHead, str);
   $$.body = string_concat($3.codeBody, codeBodyStr);
   freeRegisterT($3.reg);
   free(str);
   free(codeBodyStr);
}   //error handling
|IF BOOLEXPR')'THEN STMT ELSE STMT 
{
   output_error("expected '('");

   $$.body=string_duplicate("");
   $$.head=string_duplicate("");
}
|IF'(' ')'THEN STMT ELSE STMT
{
   output_error("Expected a boolean expression");

   $$.body=string_duplicate("");
   $$.head=string_duplicate("");
}
|WHILE'('BOOLEXPR')'STMT_BLOCK
{//1    2      3     4      5
   printf("CONTROL_STMT --> WHILE '(' BOOLEXPR ')' STMT_BLOCK\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($5.body)),sizeof(char));
   char* label = getLabel();
   char* str = (char*)calloc((200 + strlen($3.codeBody)),sizeof(char));
   codeBodyStr[0] = '\0';
   
   sprintf(codeBodyStr,"beq %s, $0, End%s:\n%s\nj Loop%s:\nEnd%s:\n", $3.reg, label, $5.body, label, label);
   sprintf(str,"Loop%s:\n", label);
   strcat(str, $3.codeBody);

   $$.head = string_concat($3.codeHead, $5.head);
   $$.body = string_concat(str, codeBodyStr);
   freeRegisterT($3.reg);
   free(str);
   free(codeBodyStr);
} //error handling
|WHILE BOOLEXPR')'STMT_BLOCK 
{
   output_error("expected '('");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");   
}
|WHILE'(' ')'STMT_BLOCK
{
   output_error("Expected a boolean expression");
 
   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
} 
|WHILE'('BOOLEXPR STMT_BLOCK 
{
   output_error("expected ')'");
 
   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|FOREACH ID ASSIGNOP NUM TILL NUM WITH STEP STMT
{//1     2      3     4    5    6   7    8    9
   printf("CONTROL_STMT --> FOREACH ID ASSIGNOP NUM TILL NUM WITH STEP STMT\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($8.body) + strlen($9.body)),sizeof(char));
   char* str = (char*)calloc((200 + strlen($8.body) + strlen($9.body)),sizeof(char));
   char errorMessage[ERROR_STRING_LEN];
   char* label = getLabel();
   char* reg;
   char* reg1;
   char* reg2;
   char* reg3;
   bool assignDone = false;
   
   symTblEntry*  symbol = lookupSymbol(*ptr2symbolTable, $2);
   if (symbol != NULL)
   {
      if(symbol->isConst == false)
      { 
         if($4.type == I && symbol->type == integer  && $6.type == I) //ints assignment
         {
            reg1 = getRegisterT();
            reg2 = getRegisterT();
            sprintf(codeBodyStr,"la %s,$s\naddi $s,$0,$d\nsw %s,0(%s)\n", reg1, $2, reg2, $4.val.ival,reg2, reg1);
            freeRegisterT(reg1);
            freeRegisterT(reg2);
            assignDone = true;
         }   
         else
         {
            if($4.type == F && symbol->type == real && $6.type == F) //floats assignment
            {
               reg1 = getRegisterF();
               reg2 = getRegisterF();
               sprintf(codeBodyStr,"la %s,$s\naddi $s,$0,$f\ns.s %s, %s\n", reg1, $2, reg2, $4.val.fval, reg2, reg1);
               freeRegisterF(reg1);
               freeRegisterF(reg2);
               assignDone = true;
            }
            else
            {
               if($4.type == I && symbol->type == real && $6.type == I) //assign an int into a float
               {
                  reg1 = getRegisterT();
                  reg2 = getRegisterT();
                  reg3 = getRegisterF();
                  sprintf(codeBodyStr,"la %s,$s\naddi $s,$0,$d\nmtc1 %s, %s\ncvt.s.w %s, %s\ns.s %s, %s\n", reg1, $2, reg2, $4.val.ival, reg2, reg3, reg3 ,reg3,reg3,reg1);
                  freeRegisterT(reg1);
                  freeRegisterT(reg2);
                  freeRegisterF(reg3);
                  assignDone = true;
               }
               else   // attempting to assign a float into an int
               {
                  sprintf(errorMessage,"Can't convert from a real number to integer");
                  output_error(errorMessage);
                  sprintf(codeBodyStr,"");
            
               }
            }
         }
      }
      else
      {
         sprintf(errorMessage,"Can't assign to a constant");
         output_error(errorMessage);
         sprintf(codeBodyStr,"");
   
      }
   }
   else
   {
      sprintf(errorMessage,"Id isn't declared");
      output_error(errorMessage);
      sprintf(codeBodyStr,"");

   }

   if (assignDone == true)
   {
      // assignment completed correctly
      if ($6.type == I)   //iterator must be int
      {   
         reg1 = getRegisterT();
         reg2 = getRegisterT();
         reg3 = getRegisterT();
              //   reg2 $2     reg1,reg2      reg3,$6.val.ival   reg2,reg3,reg1         $9.body                 label
      sprintf(str,"la %s,%s\nlw %s,0(%s)\nli $s,%d\nLOOP%s:\nnslt %s,%s,%s\nbnez $s,End%s\n$s\naddi $s,%s,%s\nj LOOP%s\nEnd%s:", reg2, $2, reg1, reg2, reg3, $6.val.ival, label, reg2, reg3, reg1, reg2, label, $9.body, reg2, reg2, $8.body, label, label);
                                          //label                reg2,label          reg2,reg2,$8.body      label
      $$.body = string_concat("codeBodyStr",str);
      $$.head = string_concat($8.head,$9.head);
      }
      else
      {   //error - iterator must be of type int !!!
         sprintf(errorMessage,"Iterator must be of type int");
         output_error(errorMessage);
   
         $$.body = string_duplicate("");
         $$.head = string_duplicate("");
      }

      freeRegisterT(reg1);
      freeRegisterT(reg2);
      freeRegisterT(reg3);
   }
   free(str);
   free(codeBodyStr);
}
|FOREACH ID ASSIGNOP NUM TILL ID WITH STEP STMT
{ //1     2   3      4     5   6   7    8   9 
   printf("CONTROL_STMT --> FOREACH ID ASSIGNOP NUM TILL ID WITH STEP STMT\n");
   $$.body = string_duplicate("");
   $$.head = string_duplicate("");

}
|FOREACH ID NUM TILL NUM WITH STEP STMT
{
   output_error("expected an assignment operation");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|FOREACH ID ASSIGNOP NUM NUM WITH STEP STMT
{
   output_error("expected '='");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|FOREACH ID ASSIGNOP NUM ID WITH STEP STMT
{
   output_error("expected '='");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|switch
{//1
   printf("CONTROL_STMT --> switch\n");
   $$.head = string_duplicate($1.codeHead);
   $$.body = string_duplicate($1.codeBody);
};


STMT_BLOCK : '{'STMTLIST'}'
{//0        1      2      3
   printf("STMT_BLOCK --> '{' STMTLIST '}'\n");
   $$.head = string_duplicate($2.head);
   $$.body = string_duplicate($2.body);
}
|STMTLIST'}'
{ //error handling
   output_error("expected '{'");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");   
}
|'{'STMTLIST 
{
   output_error("expected '}'");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");   
};


switch : SWITCH'('CHOICE')''{' CASES '}'
{//0      1    2     3      4   5   6     7
   printf("switch --> SWITCH '(' CHOICE ')' '{' CASES '}'\n");
   $$.codeBody = string_concat($3.codeBody,$6.codeBody);
   $$.codeHead = string_concat($3.codeHead,$6.codeHead);
}
|SWITCH  CHOICE')''{' CASES '}'
{ 
   output_error("expected '('");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|SWITCH'(' ')''{'CASES'}'
{ 
   output_error("expected a CHOICE");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|SWITCH'('CHOICE '{'CASES'}'
{
   output_error("expected ')'");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|SWITCH'('CHOICE')' CASES'}'
{
   output_error("expected '{'");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|SWITCH'('CHOICE')''{'CASES 
{
   output_error("expected '}'");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
};


CHOICE : ID
{//0     1
   printf("CHOICE --> ID\n");
   char codeBodyStr[200] = { 0 };
   char* reg;
   symTblEntry*  symbol = lookupSymbol(*ptr2symbolTable, $1);

   if (symbol != NULL)
   {
      if(symbol->type == integer)
      {
         sprintf(codeBodyStr,"lw $s1,%s\n", $1);
      }
      else
      {
         if(symbol->type == real)
         {
            reg = getRegisterF();
            sprintf(codeBodyStr,"l.s %s,%s\n", reg, $1);
            freeRegisterF(reg);
         }
      }
      $$.codeBody = string_duplicate(codeBodyStr);
   }
   else
   {
      output_error("Id not declared");

      $$.codeBody = string_duplicate("");
   }
   
   $$.codeHead = string_duplicate("");
}
|NUM
{//1
   printf("CHOICE --> NUM\n");
   char codeBodyStr[200] = { 0 };
   char codeHeadStr[200] = { 0 };
   char* reg;
   
   if($1.type == I)
   {
      sprintf(codeBodyStr, "li $s1,%d\n", $1.val.ival);
   }
   else
   {
      reg = getRegisterF();
      char* label = getLabel();
      sprintf(codeHeadStr,"%s: .float %f\n", label, $1.val.fval);
      sprintf(codeBodyStr,"l.s %s,%s\n", reg, label);
   }
   $$.codeHead = string_duplicate(codeHeadStr);
   $$.codeBody = string_duplicate(codeBodyStr);
};


CASES : CASE NUM':'STMTLIST BREAK';'CASES
{//0    1     2     3      4      5    6     7
   printf("CASES --> CASE NUM ':' STMTLIST BREAK ';' CASES\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($4.body)),sizeof(char));
   char* label = getLabel();
   char* reg =  getRegisterT();

   sprintf(codeBodyStr,"li %s, %d\nbne %s, $s1 ,next%s\n%s\nj end%s\nnext%s:\n", reg, $2.val.ival, reg, label, $4.body, $7.label, label);   
   $$.label = $7.label;
   $$.codeHead = string_concat($4.head, $7.codeHead);
   $$.codeBody = string_concat(codeBodyStr, $7.codeBody);
   freeRegisterT(reg);
   free(codeBodyStr);
}
|CASE NUM STMTLIST BREAK';'CASES
{
   output_error("expected ':'");
 
   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|CASE NUM':'STMTLIST BREAK CASES
{
   output_error("expected ';'");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|DEFAULT':'STMTLIST
{//1      2   3
   printf("CASES --> DEFAULT ':' STMTLIST\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($3.body)),sizeof(char));
   char* label = getLabel();

   sprintf(codeBodyStr,"default%s:\n%s\nEnd%s:", label, $3.body, label);
   $$.label = string_duplicate(label);
   $$.codeHead = string_duplicate($3.head);
   $$.codeBody = string_duplicate(codeBodyStr);
   free(codeBodyStr);
}
|DEFAULT STMTLIST
{
   output_error("expected ':'");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
};


STEP : ID ASSIGNOP ID ADDOP NUM
{//0   1   2       3   4    5
   printf("STEP : ID ASSIGNOP ID ADDOP NUM\n");
   char codeBodyStr[200] = { 0 };
   char errorMessage[ERROR_STRING_LEN];
   symTblEntry*  symbol1 = lookupSymbol(*ptr2symbolTable, $1);
   symTblEntry*  symbol2 = lookupSymbol(*ptr2symbolTable, $3);

   if (symbol1 == NULL || symbol2 == NULL)
   {
      output_error("Id not declared");

      $$.head = string_duplicate("");
      $$.body = string_duplicate("");
   }
   else
   {
      if(symbol1->type == integer && symbol2->type == integer && $5.type == I) // all ints assignment
      {
         char* reg = getRegisterT();
         if($4 == PLUS)
            sprintf(codeBodyStr,"lw, %s, %s\naddi %s, %s ,%d\nsw %s, %s\n", reg, $3, reg, reg, $5.val.ival, reg, $1);
         else
            sprintf(codeBodyStr,"lw, %s, %s\nsubi %s, %s ,%d\nsw %s, %s\n", reg, $3, reg, reg, $5.val.ival, reg, $1);
         freeRegisterT(reg);
      }
      if(symbol1->type == real && symbol2->type == real && $5.type == F)   // all floats assignment
      {
         char* reg = getRegisterF();
         if($4 == PLUS)
         sprintf(codeBodyStr,"l.s, %s, %s\nadd.s %s, %s ,%d\ns.s %s, %s\n", reg, $3, reg, reg, $5.val.fval, reg, $1);
         else
         sprintf(codeBodyStr,"l.s, %s, %s\nsub.s %s, %s ,%d\ns.s %s, %s\n", reg, $3, reg, reg, $5.val.fval, reg, $1);
         freeRegisterF(reg);                                                    
      }
   }
      
   $$.head = string_duplicate("");
   $$.body = string_duplicate(codeBodyStr);
}
|ID ID ADDOP NUM
{
   output_error("expected an assigment operation");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|ID ASSIGNOP ADDOP NUM
{
   output_error("expected an ID");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|ID ASSIGNOP ID NUM
{
   output_error("expected an operation");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|ID ASSIGNOP ID ADDOP 
{
   output_error("expected a number");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|ID ASSIGNOP ID MULOP NUM
{//1   2     3    4    5
   printf("STEP : ID ASSIGNOP ID MULOP NUM\n");
   char codeBodyStr[200] = { 0 };
   symTblEntry*  symbol1 = lookupSymbol(*ptr2symbolTable, $1);
   symTblEntry*  symbol2 = lookupSymbol(*ptr2symbolTable, $3);

   if (symbol1 == NULL || symbol2 == NULL)
   {
      output_error("Id not declared");

      $$.head = string_duplicate("");
      $$.body = string_duplicate("");
   }
   else
   {
      if(symbol1->type == integer && symbol2->type == integer && $5.type == I)   // all ints assignment
      {
         char* reg = getRegisterT();
         if($4 == MUL)
            sprintf(codeBodyStr,"la ,%s, %s\nmul %s, %s ,%d\nsw %s, %s\n", reg, $3, reg, reg, $5.val.ival, reg, $1);
         else
            sprintf(codeBodyStr,"la ,%s, %s\ndiv %s, %s ,%d\nsw %s, %s\n",reg, $3, reg, reg, $5.val.ival, reg, $1);
         freeRegisterT(reg);
      }
      if(symbol1->type == real && symbol2->type == real && $5.type == F)   // all floats assignment
      {
         char* reg = getRegisterF();
         if($4 == MUL)
            sprintf(codeBodyStr,"l.s, %s, %s\nmul.s %s, %s ,%d\ns.s %s, %s\n", reg ,$3, reg, reg, $5.val.fval, reg, $1);
         else
            sprintf(codeBodyStr,"l.s, %s, %s\ndiv.s %s, %s ,%d\ns.s %s, %s\n", reg, $3, reg, reg, $5.val.fval, reg, $1);
         freeRegisterF(reg);                                                    
      }
   }
   $$.head = string_duplicate("");
   $$.body = string_duplicate(codeBodyStr);
}
|ID ASSIGNOP MULOP NUM
{
   output_error("expected an ID");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
}
|ID ASSIGNOP ID MULOP
{
   output_error("expected a number");

   $$.body = string_duplicate("");
   $$.head = string_duplicate("");
};


BOOLEXPR : BOOLEXPR OROP BOOLTERM
{//0       1       2      3
   printf("BOOLEXPR --> BOOLEXPR OROP BOOLTERM\n");
   char codeBodyStr[200] = { 0 };
   char* tmp;
   char* reg = getRegisterT();
   char* label = getLabel();

   sprintf(codeBodyStr,"bne %s, %s, else%s\nadd %s, $0, %s\nj end%s\nelse%s: addi %s, $0, 1\nend%s", $1.reg ,$3.reg, label, reg, $1.reg, label, label, reg, label);
   $$.reg = string_duplicate(reg);
   tmp = string_concat($1.codeBody, $3.codeBody);
   $$.codeBody = string_concat(tmp, codeBodyStr);
   $$.codeHead = string_concat($1.codeHead, $3.codeHead);
   freeRegisterT($1.reg);
   freeRegisterT($3.reg);
   free(tmp);
}
|BOOLTERM
{//1
   printf("BOOLEXPR --> BOOLTERM\n");
   $$.reg = string_duplicate($1.reg);
   $$.codeBody = string_duplicate($1.codeBody);
   $$.codeHead = string_duplicate($1.codeHead);
}
|BOOLEXPR BOOLTERM
{
   output_error("expected an OROP");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
};


BOOLTERM : BOOLTERM ANDOP BOOLFACTOR
{//0      1      2      3
   printf("BOOLTERM --> BOOLTERM ANDOP BOOLFACTOR\n");
   char codeBodyStr[200] = { 0 };
   char* tmp;
   char* reg = getRegisterT();
   
   sprintf(codeBodyStr,"mul %s, %s, %s\n", reg, $1.reg ,$3.reg);
   $$.reg = string_duplicate(reg);
   tmp = string_concat($1.codeBody, $3.codeBody);
   $$.codeBody = string_concat(tmp, codeBodyStr);
   $$.codeHead = string_concat($1.codeHead,$3.codeHead);
   freeRegisterT($1.reg);
   freeRegisterT($3.reg);
   free(tmp);
}
|BOOLTERM BOOLFACTOR
{
   output_error("expected an ANDOP");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|BOOLFACTOR
{//1
   printf("BOOLTERM --> BOOLFACTOR\n");
   $$.reg = string_duplicate($1.reg);
   $$.codeBody = string_duplicate($1.codeBody);
   $$.codeHead = string_duplicate($1.codeHead);
};


BOOLFACTOR : '!''('BOOLFACTOR')'
{//0         1   2   3        4
   /*Meaning not BOOLFACTOR*/
   printf("BOOLFACTOR -->  '!' '(' BOOLFACTOR ')'\n");
   char codeBodyStr[200] = { 0 };
   char* reg = getRegisterT();
   
   sprintf(codeBodyStr,"li %s ,1\nsub %s, %s, %s\n", reg, $3.reg, reg, $3.reg);
   $$.reg= $3.reg;
   $$.codeBody = string_concat($3.codeBody, codeBodyStr);
   $$.codeHead = $3.codeHead;
   freeRegisterT(reg);
}
|'!' BOOLFACTOR')'
{
   output_error("expected an '('");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|'!''(' ')'
{
   output_error("expected a BOOLFACTOR");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|'!''('BOOLFACTOR 
{
   output_error("expected a ')'");

   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|EXPRESSION RELOP EXPRESSION
{//1         2      3
   printf("BOOLFACTOR -->  EXPRESSION  RELOP  EXPRESSION\n");
   char codeBodyStr[200] = { 0 };
   char tmpBody[200] = { 0 };
   char tmpHead[200] = { 0 };
   char* reg = getRegisterT();

   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"int") == 0) //RELOP between ints
   {
      if($2 == EQ)        sprintf(codeBodyStr,"seq %s, %s, %s\n", reg, $1.reg, $3.reg);
      else if($2 == NEQ)  sprintf(codeBodyStr,"sne %s, %s, %s\n", reg, $1.reg, $3.reg);
      else if($2 == LT)   sprintf(codeBodyStr,"slt %s, %s, %s\n", reg, $1.reg, $3.reg);
      else if($2 == GT)   sprintf(codeBodyStr,"sgt %s, %s, %s\n", reg, $1.reg, $3.reg);
      else if($2 == LTEQ) sprintf(codeBodyStr,"sle %s, %s, %s\n", reg, $1.reg, $3.reg);
      else if($2 == GTEQ) sprintf(codeBodyStr,"sge %s, %s, %s\n", reg, $1.reg, $3.reg);

      freeRegisterT($1.reg);
      freeRegisterT($3.reg);         
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"float") == 0) //RELOP between floats
   {
      char* label = getLabel();
      if($2 == EQ)        sprintf(codeBodyStr,"c.eq.s %s, %s\nbc1f else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg ,$3.reg, label, reg, label, label, reg, label);
      else if($2 == NEQ)  sprintf(codeBodyStr,"c.eq.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg, $3.reg, label, reg, label, label, reg, label);
      else if($2 == LT)   sprintf(codeBodyStr,"c.lt.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $3.reg, $1.reg, label, reg, label, label, reg, label);
      else if($2 == GT)   sprintf(codeBodyStr,"c.lt.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg, $3.reg, label, reg, label, label, reg, label);
      else if($2 == LTEQ) sprintf(codeBodyStr,"c.le.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $3.reg, $1.reg, label, reg, label, label, reg, label);
      else if($2 == GTEQ) sprintf(codeBodyStr,"c.le.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg, $3.reg, label, reg, label, label, reg, label);

      freeRegisterF($1.reg);
      freeRegisterF($3.reg);
   }

   $$.reg = string_duplicate(reg);
   strcat(tmpBody,$1.codeBody);
   strcat(tmpBody,$3.codeBody);
   strcat(tmpBody,codeBodyStr);
   strcat(tmpHead,$1.codeHead);
   strcat(tmpHead,$3.codeHead);
   $$.codeBody = string_duplicate(tmpBody);
   $$.codeHead = string_duplicate(tmpHead);
};


EXPRESSION : EXPRESSION ADDOP TERM 
{//0         1         2      3
   printf("EXPRESSION --> EXPRESSION  ADDOP  TERM\n");
   char codeBodyStr[200] = { 0 };
   char* tmp;
   
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"int") == 0) //ADDOP between ints
   {
      if($2 == PLUS) sprintf(codeBodyStr,"add %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      else           sprintf(codeBodyStr,"sub %s, %s ,%s\n", $3.reg, $1.reg, $3.reg);
 
      freeRegisterT($1.reg);
      $$.type = string_duplicate("int");
      $$.reg = string_duplicate($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"int") == 0)   //ADDOP between a float and an int
   {
      char* reg = getRegisterF();
      if($2 == PLUS) sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nadd.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
      else           sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nsub.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
 
      freeRegisterF(reg);
      freeRegisterT($3.reg);
      $$.type = string_duplicate("float");
      $$.reg = string_duplicate($1.reg);
   }
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"float") == 0)   //ADDOP between an int and a float
   {
      char* reg = getRegisterF();
      if($2 == PLUS) sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nadd.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, $3.reg, reg);
      else           sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nsub.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, reg, $3.reg);

      freeRegisterF(reg);
      freeRegisterT($1.reg);
      $$.type = string_duplicate("float");
      $$.reg = string_duplicate($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"float") == 0)   //ADDOP between floats
   {
      if($2 == PLUS)  sprintf(codeBodyStr,"add.s %s, %s ,%s\n", $3.reg, $1.reg, $3.reg);
      else            sprintf(codeBodyStr,"sub.s %s, %s ,%s\n", $3.reg, $1.reg, $3.reg);

      freeRegisterF($1.reg);
      $$.type = string_duplicate("float");
      $$.reg = string_duplicate($3.reg);
   }         
   
   tmp = string_concat($1.codeBody, $3.codeBody);
   $$.codeBody= string_concat(tmp, codeBodyStr);
   $$.codeHead= string_concat($1.codeHead,$3.codeHead);
   free(tmp);
}
|TERM
{//1
   printf("EXPRESSION --> TERM\n");
   $$.reg =  string_duplicate($1.reg);
   $$.codeBody = string_duplicate($1.codeBody);
   $$.codeHead = string_duplicate($1.codeHead);
   $$.type = string_duplicate($1.type);
   dbg_print($$.type);
};


TERM : TERM MULOP FACTOR
{//0   1   2      3
   printf("TERM --> TERM MULOP FACTOR\n");
   char codeBodyStr[200] = { 0 };
   char* tmp;
   
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"int") == 0)   //MULOP between ints
   {
      if($2 == MUL) sprintf(codeBodyStr,"mul %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      else          sprintf(codeBodyStr,"div %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);

      freeRegisterT($1.reg);
      $$.type = string_duplicate("int");
      $$.reg = string_duplicate($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"int") == 0) //MULOP between a float and an int
   {
      char* reg = getRegisterF();

      if($2 == MUL) sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nmul.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
      else          sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ndiv.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);

      freeRegisterF(reg);
      freeRegisterT($3.reg);
      $$.type = string_duplicate("float");
      $$.reg = string_duplicate($1.reg);
   }
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"float") == 0)   //MULOP between an int and a float
   {
      char* reg = getRegisterF();

      if($2 == MUL) sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nmul.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, $3.reg, reg);
      else          sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ndiv.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, $3.reg, reg);

      freeRegisterF(reg);
      freeRegisterT($1.reg);
      $$.type = string_duplicate("float");
      $$.reg = string_duplicate($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"float") == 0)   //MULOP between floats
   {
      if($2 == MUL) sprintf(codeBodyStr,"mul.s %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      else          sprintf(codeBodyStr,"div.s %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);

      freeRegisterF($1.reg);
      $$.type = string_duplicate("float");
      $$.reg = string_duplicate($3.reg);
   }
   
   tmp = string_concat($1.codeBody, $3.codeBody);
   $$.codeBody = string_concat(tmp, codeBodyStr);
   $$.codeHead = string_concat($1.codeHead, $3.codeHead);
   free(tmp);
}
|TERM FACTOR
{
   output_error("expected a MULOP");
   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|TERM MULOP
{
   output_error("expected a FACTOR");
   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|FACTOR
{//1
   printf("TERM --> FACTOR\n");
   $$.reg =  string_duplicate($1.reg);
   $$.codeBody = string_duplicate($1.codeBody);
   $$.codeHead = string_duplicate($1.codeHead);
   $$.type = string_duplicate($1.type);
   dbg_print($$.type);
};


FACTOR : '('EXPRESSION')'
{//0     1      2      3
   printf("FACTOR --> '(' EXPRESSION ')'\n");
   $$.reg =  string_duplicate($2.reg);
   $$.codeBody = string_duplicate($2.codeBody);
   $$.codeHead = string_duplicate($2.codeHead);
   $$.type = string_duplicate($2.type);
   dbg_print($$.type);
}
|'('EXPRESSION
{
   output_error("expected a ')'");
   $$.codeBody = string_duplicate("");
   $$.codeHead = string_duplicate("");
}
|ID
{//1
   printf("FACTOR --> ID\n");
   char codeBodyStr[200] = { 0 };
   char* reg;
   char* label = getLabel();
   dbg_print(label);
   dbg_print($1);
   symTblEntry* symbol = lookupSymbol(*ptr2symbolTable, $1);
   
   if(symbol != NULL)
   {
      if(symbol->type == integer)
      {
         reg = getRegisterT();
         sprintf(codeBodyStr,"lw %s, %s\n", reg, $1);
      }
      if(symbol->type == real)
      {
         reg = getRegisterF();
         sprintf(codeBodyStr,"l.s %s, %s\n", reg, $1);
      }
      if(symbol->type == string)
      {
         reg = $1;
	 dbg_print(reg);
      }
      $$.reg = string_duplicate(reg);
      $$.type = EnumType2charType(symbol->type);
      $$.codeBody = string_duplicate(codeBodyStr);
      $$.codeHead = string_duplicate("");
   }
   else
   {
      output_error("Id not declared");
      $$.codeBody = string_duplicate("");
      $$.codeHead = string_duplicate("");
   }
}
|NUM
{//1
   printf("FACTOR --> NUM\n");
   char codeBodyStr[200] = { 0 };
   char codeHeadStr[200] = { 0 };
   char* reg;
   
   if($1.type == I) //int
   {
      reg = getRegisterT();
      sprintf(codeBodyStr,"addi %s, $0, %d\n", reg, $1.val.ival);
      $$.type = string_duplicate("int");

   }
   else   //$1.type == F
   {
      reg = getRegisterF();
      char* label = getLabel();
      sprintf(codeHeadStr,"%s: .float %f\n", label, $1.val.fval);
      sprintf(codeBodyStr,"l.s %s, %s\n", reg, label);
      $$.type = string_duplicate("float");
   }
   $$.reg = string_duplicate(reg);
   $$.codeHead = string_duplicate(codeHeadStr);
   $$.codeBody = string_duplicate(codeBodyStr);
};
%%

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
      inputFileName = string_duplicate(argv[1]);
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
   do
   {
      fgets(line, 500, yyin);
      fprintf(listFile, "%d: %s", linesCounter++, line);
   }while(!feof(yyin));
   fprintf(listFile, "\n");
   printf("Copying input code to listing file completed\n");
   fclose(listFile);

// ########  End of copy to list file section  ########

   fseek(yyin, 0, SEEK_SET); // moving cursor to the beginning for lexical analysis
   printf("Starting parsing process\n");
   yyparse();
   printf("Parsing process completed\n");

   // printf("%s", mipsCode);

   if (hasErrors == false) // no errors were found during parsing process -> creating MIPS file
   {
      if (strlen(mipsCode) == 0)
      {
         fprintf(stderr, "Unexpected error occurred. MIPS file can not be created\n");
      }
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
   {
      fprintf(stderr, "Errors were found. See listing file for details\n");
   }

   fclose (yyin);
   printf("Destroying symbol table\n");
   printf("Compilation process is done\n");
   destroySymTable(*ptr2symbolTable);
   return 0;
}

void yyerror (char *s)
{}
