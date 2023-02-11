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
#define SYM_TBL_ROWS_COUNT 10

extern int yylex();
extern int yylineno;
void yyerror (char *s);

// ############### Symbol table definitions ####################
typedef enum Type 
{
   integer, 
   floating, 
   string
}Type;

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
   {
      symbolTbl->symbolTableHead[i] = NULL;
   }
   return symbolTbl;
}

/*
* Calculates a hash value for a given lexeme
*/
int getHash(char* lexeme)
{
   if (lexeme == NULL || strlen(lexeme) < 1)
   {
      return -1;
   }
   return strlen(lexeme) % SYM_TBL_ROWS_COUNT;
}


/*
* Searches for an entry in a symbol table in an effective hash-based algorithm
*/
symTblEntry* lookup(symTbl* symTbl, char* lexeme)
{
   if(symTbl == NULL || lexeme == NULL)
   {
      return NULL;
   }

   int hashValue = getHash(lexeme);
   if (hashValue == -1)
   {
      fprintf(stderr, "Failed to get hash value\n");
      return NULL;
   }
   if (hashValue > 0 && hashValue < SYM_TBL_ROWS_COUNT)
   {   
      symTblEntry* ptr = symTbl->symbolTableHead[hashValue];   // lookup only in the line matched  by the hash
      while (ptr != NULL)
      {   
         if (0 == strcmp(ptr->name, lexeme))
         {
            return ptr;
         }
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
      
   else   //otherwise, add newEntry to the Head of the appropriate list
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
   Type newType;      // enum Type {integer, floating, string}
   if (type == NULL)   // for argument protection only
   {
      newType = integer;
   }
   else if (0 == strcmp(type, "int")) 
   {
      newType = integer;
   }
   else if (0 == strcmp(type, "float")) 
   {
      newType = floating;
   }
   else if (0 == strcmp(type, "string")) 
   {
      newType = string;
   }

   return newType;
}


/*
* Converts Enum Type of symbol table to char* type
*/
char* EnumType2charType(Type type)
{
   char* newType = NULL;      // enum Type {integer, floating, string}

   if (type == integer) newType = strdup("int");
   else if (type == floating) newType = strdup("float");
   else if (type == string) newType = strdup("string");
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

   Type newType = charType2EnumType(type);      // Converting char* type to Enum Type
   symTblEntry* newSymbol = createSymTblEntery(symTbl, lexeme, newType, isConst, hashValue);

   if (newSymbol == NULL)
   {
      return;
   }
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

%token <nVal>  NUM
%token <op> ADDOP MULOP ASSIGNOP OROP ANDOP RELOP
%token <sval> ID SENTENCE 
%token START BREAK CASE FINAL DEFAULT DO ELSE END FOREACH IF INT DCL OUT REAL READ SWITCH TILL WHILE WITH STRING VAR PROGRAM

%type <decl> EXPRESSION TERM FACTOR TYPE LIST DECLARATIONS DECLARLIST DECL CDECL switch CHOICE CASES BOOLEXPR BOOLTERM BOOLFACTOR
%type <code> STMTLIST STMT ASSIGNMENT_STMT CONTROL_STMT STMT_BLOCK OUT_STMT READ_STMT STEP
%start program

%left addop
%left mulop
%left orop
%left andop
%left relop
%right assignop
%%
program : PROGRAM ID START DECLARATIONS STMTLIST END
{// 0      1     2     3     4         5      6
	printf("PROGRAM --> PROGRAM ID START DECLARATIONS STMTLIST END\n");
	char* str = (char*)calloc(1000, sizeof(char));
   
	if($5.head == NULL) $5.head = "";
	if($4.codeBody == NULL) $4.codeBody = "";
	sprintf(str,".data\n%s\n%s\nstrBuff: .space 200\n.text\nProgram:\n%s\n%s\n", $4.codeHead, $5.head, $4.codeBody, $5.body);
	mipsCode = str;
}


DECLARATIONS : DCL DECLARLIST CDECL
{//0         1      2      3
   printf("DECLARATIONS --> DCL DECLARLIST CDECL\n");
   $$.codeHead = strdup(strcat($2.codeHead, $3.codeHead));
   $$.codeBody = strdup($3.codeBody);
}
|
{
   printf("DECLARATIONS --> epsilon\n");
   $$.codeHead = strdup("");
   $$.codeBody = strdup("");
};


DECLARLIST : DECLARLIST DECL
{
   printf("DECLARLIST --> DECLARLIST DECL\n");
   $$.codeHead = strConcat($1.codeHead, $2.codeHead);
}
|DECL
{
   printf("DECLARLIST --> DECL\n");
   $$.codeHead = strdup($1.codeHead);
};


DECL :  TYPE':'LIST
{//0   1     2      3
   printf("DECL -->  TYPE ':' LIST\n");
   char* tempCodeHeadStr = (char*)calloc(200, sizeof(char));
   char* codeHeadStr = (char*)calloc(200, sizeof(char));
   char errorMessage[ERROR_STRING_LEN]; 
   int i;
   
   for(i = 0 ; i < 5 ; i++)
   {
      if($3.IDarray[i] != NULL)
      {
         if(lookup(*ptr2symbolTable, $3.IDarray[i]) == NULL)
         {
            addSymbol(*ptr2symbolTable, $3.IDarray[i], $1.type, false);
            if(strcmp($1.type,"string") == 0)
            {
               sprintf(tempCodeHeadStr,"%s: .space 200\n",$3.IDarray[i]);
               strcat(codeHeadStr,tempCodeHeadStr);
            }
            else
            {
               sprintf(tempCodeHeadStr,"%s: .space 8\n",$3.IDarray[i]);
               strcat(codeHeadStr,tempCodeHeadStr);
            }
         }
         else 
         {   sprintf(errorMessage, "Duplicted declaration. %s already declared\n", $3.IDarray[i]);
            outputError(errorMessage);
            sprintf(codeHeadStr,"");
            hasErrors = true;
         }
      }
   }
   $$.codeHead = strdup(codeHeadStr);
   free(codeHeadStr);
   free(tempCodeHeadStr);
}
|TYPE LIST 
{
outputError("Expected ':' ");
hasErrors = true;
$$.codeHead = strdup("");
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
|ID { outputError("Expected ';' "); hasErrors = true; }
|ID LIST
{
   outputError("Expected ',' ");
   hasErrors = true;
};


TYPE : INT
{//0    1
   printf("TYPE --> INT\n");
   $$.type = strdup("int");
}
|REAL
{//1
   printf("TYPE --> REAL\n");
   $$.type = strdup("float");
}
|STRING
{//1
   printf("TYPE --> STRING\n");
   $$.type = strdup("string");
};


CDECL : FINAL TYPE ID ASSIGNOP NUM';'CDECL
{//0   1      2   3   4      5  6   7
   printf("CDECL --> FINAL TYPE ID ASSIGNOP NUM ';' CDECL\n");
   char* codeBodyStr = (char*)calloc(200, sizeof(char));
   char* codeHeadStr = (char*)calloc(200, sizeof(char));
   char errorMessage[ERROR_STRING_LEN];
   if(lookup(*ptr2symbolTable, $3) == NULL) //id not in symbol table
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
      sprintf(errorMessage, "Duplicted declaration. %s already declared\n", $3);
      outputError(errorMessage);
      sprintf(codeHeadStr,"");
      hasErrors = true;
   }
   $$.codeHead = strConcat(codeHeadStr, $7.codeHead);
   $$.codeBody = strConcat(codeBodyStr, $7.codeBody);
}
|FINAL TYPE ID ASSIGNOP NUM CDECL
{
   outputError("Expected ';' ");
   hasErrors = true;
   $$.codeHead = strdup("");
   $$.codeBody = strdup("");
}
| 
{
   printf("CDECL --> epsilon\n");
   $$.codeHead = strdup("");
   $$.codeBody = strdup("");
};


STMTLIST : STMTLIST STMT
{//0      1       2
   printf("STMTLIST --> STMTLIST STMT\n");
   $$.body = strConcat($1.body, $2.body);
}
|
{
   printf("STMTLIST --> epsilon\n");
   $$.body = strdup("");
};


STMT : ASSIGNMENT_STMT
{//0   1
   printf("STMT --> ASSIGNMENT_STMT\n");
   $$.head = strdup($1.head);
   $$.body = strdup($1.body);
}
|ID ASSIGNOP SENTENCE';'
{//1   2      3      4
   printf("STMT --> ID ASSIGNOP SENTENCE ';'\n");
   
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* codeHeadStr = (char*)calloc(200,sizeof(char));
   char* temp;
   char errorMessage[ERROR_STRING_LEN];

   char* reg = getRegisterT();
   char* label = getLabel();
   symTblEntry*  symbol = lookup(*ptr2symbolTable, $1);
   if (symbol != NULL)
   {   
      if(symbol->isConst == false)
      {
         if(symbol->type == string)
         {   
            temp = (char*)calloc(1, sizeof(char) * (strlen($3)+1));
            temp = strcpy(temp, $3);
            symbol->value.sval = temp;
            
            sprintf(codeHeadStr,"%s: .asciiz %s\n", label, $3);
            sprintf(codeBodyStr,"lw %s, %s\nsw %s, %s\n", reg, label, reg, $1);
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
   $$.head = strdup(codeHeadStr);
   $$.body = strdup(codeBodyStr);
   free(codeHeadStr);
   free(codeBodyStr);
}
|CONTROL_STMT
{   //1
   printf("STMT --> CONTROL_STMT\n");
   $$.head = strdup($1.head);
   $$.body = strdup($1.body);
}
|READ_STMT
{ //1
   printf("STMT --> READ_STMT\n");
   $$.body = strdup($1.body);
}
|OUT_STMT
{ //1 
   printf("STMT --> OUT_STMT\n");
   $$.head = strdup($1.head);
   $$.body = strdup($1.body);
}
|STMT_BLOCK
{ //1
   printf("STMT --> STMT_BLOCK\n");
   $$.head = strdup($1.head);
   $$.body = strdup($1.body);
}
|ID ASSIGNOP SENTENCE
{ 
   outputError("Expected ';' ");
   hasErrors = true;
   $$.head = strdup("");
   $$.body = strdup("");
};


OUT_STMT : OUT'('EXPRESSION')'';'
{//0      1   2   3         4   5
   printf("OUT_STMT --> OUT '(' EXPRESSION ')' ';'\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   
   if(strcmp($3.type, "int") == 0)   //print an int
      sprintf(codeBodyStr,"li $v0,1\nmove $a0,%s\n syscall\n",$3.reg);
   
   if(strcmp($3.type, "float") == 0)   //print a float
      sprintf(codeBodyStr,"li $v0,2\nmov.s $f12,%s\n syscall\n",$3.reg);
   
   $$.head = $3.codeHead;
   $$.body = strConcat($3.codeBody, codeBodyStr);
   free(codeBodyStr);
}
|OUT EXPRESSION')'';'
{
   outputError("Expected '(' ");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|OUT'('EXPRESSION ';'
{
   outputError("Expected ')'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}

|OUT'('EXPRESSION')'
{ 
   outputError("expected ';' ");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|OUT'('SENTENCE')'';'
{//1   2   3      4   5
   printf("OUT_STMT --> OUT '(' SENTENCE ')' ';'\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* codeHeadStr = (char*)calloc(200,sizeof(char));
   char* label = getLabel();
   
   sprintf(codeHeadStr, "%s: .asciiz %s\n", label, $3);
   sprintf(codeBodyStr, "la $a0,%s\nli $v0,4\nsyscall\n", label); //print a string
   
   $$.head = strdup(codeHeadStr);
   $$.body = strdup(codeBodyStr);
   free(codeHeadStr);
   free(codeBodyStr);
}   // error handling
|OUT SENTENCE')'';'
{
   outputError("Expected '('");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|OUT'('SENTENCE ';'
{
   outputError("Expected ')'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}

|OUT'('SENTENCE')'
{
   outputError("expected ';'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
};


READ_STMT : READ'('ID')'';'
{//0      1     2     3    4    5
   printf("READ_STMT --> READ '(' ID ')' ';'\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char errorMessage[ERROR_STRING_LEN];

   symTblEntry*  symbol = lookup(*ptr2symbolTable, $3);
   if (symbol != NULL)
   {
      if(symbol->isConst == false)
      {
         if(symbol->type == integer)   //read int
            sprintf(codeBodyStr,"li $v0,5\nsyscall\nsw $v0, %s\n", symbol->name);

         if(symbol->type == floating) //read float
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
   $$.body = strdup(codeBodyStr);
   free(codeBodyStr);
}
|READ ID')'';'
{
   outputError("expected '('");
   hasErrors = true;
   $$.body=strdup("");
}
|READ'('ID ';'
{
   outputError("expected ')'");
   hasErrors = true;
   $$.body=strdup("");
}
|READ'('ID')'
{
   outputError("expected ';'");
   hasErrors = true;
   $$.body=strdup("");
};


ASSIGNMENT_STMT : ID ASSIGNOP EXPRESSION';'
{//0           1      2      3       4
   printf("ASSIGNMENT_STMT --> ID ASSIGNOP EXPRESSION';'\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char errorMessage[ERROR_STRING_LEN];
   char* reg;

   symTblEntry*  symbol = lookup(*ptr2symbolTable, $1);
   if (symbol != NULL)
   {
      if(symbol->isConst == false)
      { 
         if(strcmp($3.type,"int") == 0 && symbol->type == integer) //ints assignment
         {
            sprintf(codeBodyStr,"sw %s, %s\n", $3.reg, $1);
            freeRegisterT($3.reg);
         }   
         else if(strcmp($3.type,"float") == 0 && symbol->type == floating) //floats assignment
         {
            sprintf(codeBodyStr,"s.s %s, %s\n", $3.reg, $1);
            freeRegisterF($3.reg);
         }
            else
            {
               if(strcmp($3.type,"int")==0 && symbol->type == floating) //assign an int into a float
               {
                  reg = getRegisterF();
                  sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ns.s %s, %s\n", $3.reg, reg, reg, reg, reg, $1);
                  freeRegisterT($3.reg);
                  freeRegisterF(reg);
               }
               else   // attempting to assign a float into an int
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

   $$.head = strdup($3.codeHead);
   $$.body = strConcat($3.codeBody, codeBodyStr);
   free(codeBodyStr);
}
|ID ASSIGNOP EXPRESSION
{
   outputError("expected ';'");
   hasErrors = true;
   $$.head = strdup("");
   $$.body = strdup("");
};


CONTROL_STMT : IF'('BOOLEXPR')'STMT ELSE STMT
{//0         1   2   3      4   5    6     7
   printf("CONTROL_STMT --> IF '(' BOOLEXPR ')' STMT ELSE STMT\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($5.body)+ strlen($7.body)),sizeof(char));
   char* label = getLabel();
   char* str;
   
   sprintf(codeBodyStr,"beq %s,$0,Else%s\n%s\nj End%s\nElse%s:\n%s\nEnd%s:\n", $3.reg, label, $5.body, label, label ,$7.body, label);   
   str = strConcat($5.head, $7.head);
   $$.head = strConcat($3.codeHead, str);
   $$.body = strConcat($3.codeBody, codeBodyStr);
   freeRegisterT($3.reg);
   free(str);
   free(codeBodyStr);
}   //error handling
|IF BOOLEXPR')'STMT ELSE STMT 
{
   outputError("expected '('");
   hasErrors = true;
   $$.body=strdup("");
   $$.head=strdup("");
}
|IF'(' ')'STMT ELSE STMT
{
   outputError("Expected a boolean expression");
   hasErrors = true;
   $$.body=strdup("");
   $$.head=strdup("");
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

   $$.head = strConcat($3.codeHead, $5.head);
   $$.body = strConcat(str, codeBodyStr);
   freeRegisterT($3.reg);
   free(str);
   free(codeBodyStr);
} //error handling
|WHILE BOOLEXPR')'STMT_BLOCK 
{
   outputError("expected '('");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");   
}
|WHILE'(' ')'STMT_BLOCK
{
   outputError("Expected a boolean expression");
   hasErrors = true; 
   $$.body = strdup("");
   $$.head = strdup("");
} 
|WHILE'('BOOLEXPR STMT_BLOCK 
{
   outputError("expected ')'");
   hasErrors = true; 
   $$.body = strdup("");
   $$.head = strdup("");
}
|FOREACH ID ASSIGNOP NUM':'NUM WITH STEP STMT
{//1     2      3      4  5   6   7    8    9
   printf("CONTROL_STMT --> FOREACH ID ASSIGNOP NUM ':' NUM WITH STEP STMT\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($8.body) + strlen($9.body)),sizeof(char));
   char* str = (char*)calloc((200 + strlen($8.body) + strlen($9.body)),sizeof(char));
   char errorMessage[ERROR_STRING_LEN];
   char* label = getLabel();
   char* reg;
   char* reg1;
   char* reg2;
   char* reg3;
   bool assignDone = false;
   
   symTblEntry*  symbol = lookup(*ptr2symbolTable, $2);
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
            if($4.type == F && symbol->type == floating && $6.type == F) //floats assignment
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
               if($4.type == I && symbol->type == floating && $6.type == I) //assign an int into a float
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
      if ($6.type == I)   //iterator must be int
      {   
         reg1 = getRegisterT();
         reg2 = getRegisterT();
         reg3 = getRegisterT();
              //   reg2 $2     reg1,reg2      reg3,$6.val.ival   reg2,reg3,reg1         $9.body                 label
      sprintf(str,"la %s,%s\nlw %s,0(%s)\nli $s,%d\nLOOP%s:\nnslt %s,%s,%s\nbnez $s,End%s\n$s\naddi $s,%s,%s\nj LOOP%s\nEnd%s:", reg2, $2, reg1, reg2, reg3, $6.val.ival, label, reg2, reg3, reg1, reg2, label, $9.body, reg2, reg2, $8.body, label, label);
                                          //label                reg2,label          reg2,reg2,$8.body      label
      $$.body = strConcat("codeBodyStr",str);
      $$.head = strConcat($8.head,$9.head);
      }
      else
      {   //error - iterator must be of type int !!!
         sprintf(errorMessage,"Iterator must be of type int");
         outputError(errorMessage);
         hasErrors = true;
         $$.body = strdup("");
         $$.head = strdup("");
      }

      freeRegisterT(reg1);
      freeRegisterT(reg2);
      freeRegisterT(reg3);
   }
   free(str);
   free(codeBodyStr);
}
|FOREACH ID ASSIGNOP NUM':'ID WITH STEP STMT
{ //1      2   3      4  5  6   7      8   9 // @@@
   printf("CONTROL_STMT --> FOREACH ID ASSIGNOP NUM':'ID WITH STEP STMT\n");
   $$.body = strdup("");
   $$.head = strdup("");

}
|FOREACH ID NUM':'NUM WITH STEP STMT
{
   outputError("expected an assignment operation");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|FOREACH ID ASSIGNOP NUM NUM WITH STEP STMT
{
   outputError("expected ':'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|FOREACH ID ASSIGNOP NUM ID WITH STEP STMT
{
   outputError("expected ':'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|DO STMT_BLOCK TILL'('BOOLEXPR')' 
{//1   2      3     4      5      6
/*do the block until BOOLEXPR is true*/
   printf("CONTROL_STMT --> DO STMT_BLOCK TILL '(' BOOLEXPR ')'\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($2.body)),sizeof(char));
   char* label = getLabel();
   char* str = (char*)calloc((200 + strlen($5.codeBody)),sizeof(char));

   //sprintf(codeBodyStr,"beq %s, $0, End%s\n%s\nj Loop%s\nEnd%s:\n", $5.reg, label, $2.body, label, label);
   sprintf(codeBodyStr,"bneq %s, $0, End%s\n%s\nj Loop%s\nEnd%s:\n", $5.reg, label, $2.body, label, label);
   sprintf(str,"Loop%s:\n",label);
   strcat(str,$5.codeBody);
   $$.head = strConcat($5.codeHead,$2.head);
   $$.body = strConcat(str,codeBodyStr);
   freeRegisterT($5.reg);
   free(str);
   free(codeBodyStr);
}
|DO STMT_BLOCK TILL BOOLEXPR')'
{
   outputError("expected '('");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");   
}
|DO STMT_BLOCK TILL'(' ')'
{
   outputError("Expected a boolean expression");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
} 
|DO STMT_BLOCK TILL'('BOOLEXPR 
{   
   outputError("expected ')'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");   
}
|switch
{//1
   printf("CONTROL_STMT --> switch\n");
   $$.head = strdup($1.codeHead);
   $$.body = strdup($1.codeBody);
};


STMT_BLOCK : '{'STMTLIST'}'
{//0        1      2      3
   printf("STMT_BLOCK --> '{' STMTLIST '}'\n");
   $$.head = strdup($2.head);
   $$.body = strdup($2.body);
}
|STMTLIST'}'
{ //error handling
   outputError("expected '{'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");   
}
|'{'STMTLIST 
{
   outputError("expected '}'");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");   
};


switch : SWITCH'('CHOICE')''{' CASES '}'
{//0      1    2     3      4   5   6     7
   printf("switch --> SWITCH '(' CHOICE ')' '{' CASES '}'\n");
   $$.codeBody = strConcat($3.codeBody,$6.codeBody);
   $$.codeHead = strConcat($3.codeHead,$6.codeHead);
}
|SWITCH  CHOICE')''{' CASES '}'
{ 
   outputError("expected '('");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|SWITCH'(' ')''{'CASES'}'
{ 
   outputError("expected a CHOICE");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|SWITCH'('CHOICE '{'CASES'}'
{
   outputError("expected ')'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|SWITCH'('CHOICE')' CASES'}'
{
   outputError("expected '{'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|SWITCH'('CHOICE')''{'CASES 
{
   outputError("expected '}'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
};


CHOICE : ID
{//0     1
   printf("CHOICE --> ID\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* reg;
   symTblEntry*  symbol = lookup(*ptr2symbolTable, $1);

   if (symbol != NULL)
   {
      if(symbol->type == integer)
      {
         sprintf(codeBodyStr,"lw $s1,%s\n", $1);
      }
      else
      {
         if(symbol->type == floating)
         {
            reg = getRegisterF();
            sprintf(codeBodyStr,"l.s %s,%s\n", reg, $1);
            freeRegisterF(reg);
         }
      }
      $$.codeBody = strdup(codeBodyStr);
   }
   else
   {
      outputError("Id not declared");
      hasErrors = true;
      $$.codeBody = strdup("");
   }
   
   $$.codeHead = strdup("");
   free(codeBodyStr);
}
|NUM
{//1
   printf("CHOICE --> NUM\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* codeHeadStr = (char*)calloc(200,sizeof(char));
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
   $$.codeHead = strdup(codeHeadStr);
   $$.codeBody = strdup(codeBodyStr);
   free(codeBodyStr);
   free(codeHeadStr);
};


CASES : CASE NUM':'STMTLIST BREAK';'CASES
{//0    1     2     3      4      5    6     7
   printf("CASES --> CASE NUM ':' STMTLIST BREAK ';' CASES\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($4.body)),sizeof(char));
   char* label = getLabel();
   char* reg =  getRegisterT();

   sprintf(codeBodyStr,"li %s, %d\nbne %s, $s1 ,next%s\n%s\nj end%s\nnext%s:\n", reg, $2.val.ival, reg, label, $4.body, $7.label, label);   
   $$.label = $7.label;
   $$.codeHead = strConcat($4.head, $7.codeHead);
   $$.codeBody = strConcat(codeBodyStr, $7.codeBody);
   freeRegisterT(reg);
   free(codeBodyStr);
}
|CASE NUM STMTLIST BREAK';'CASES
{
   outputError("expected ':'");
   hasErrors = true; 
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|CASE NUM':'STMTLIST BREAK CASES
{
   outputError("expected ';'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|DEFAULT':'STMTLIST
{//1      2   3
   printf("CASES --> DEFAULT ':' STMTLIST\n");
   char* codeBodyStr = (char*)calloc((200 + strlen($3.body)),sizeof(char));
   char* label = getLabel();

   sprintf(codeBodyStr,"default%s:\n%s\nEnd%s:", label, $3.body, label);
   $$.label = strdup(label);
   $$.codeHead = strdup($3.head);
   $$.codeBody = strdup(codeBodyStr);
   free(codeBodyStr);
}
|DEFAULT STMTLIST
{
   outputError("expected ':'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
};


STEP : ID ASSIGNOP ID ADDOP NUM
{//0   1   2      3   4    5
   printf("STEP : ID ASSIGNOP ID ADDOP NUM\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char errorMessage[ERROR_STRING_LEN];
   symTblEntry*  symbol1 = lookup(*ptr2symbolTable, $1);
   symTblEntry*  symbol2 = lookup(*ptr2symbolTable, $3);

   if (symbol1 == NULL || symbol2 == NULL)
   {
      outputError("Id not declared");
      hasErrors = true;
      $$.head = strdup("");
      $$.body = strdup("");
      free(codeBodyStr);
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
      if(symbol1->type == floating && symbol2->type == floating && $5.type == F)   // all floats assignment
      {
         char* reg = getRegisterF();
         if($4 == PLUS)
         sprintf(codeBodyStr,"l.s, %s, %s\nadd.s %s, %s ,%d\ns.s %s, %s\n", reg, $3, reg, reg, $5.val.fval, reg, $1);
         else
         sprintf(codeBodyStr,"l.s, %s, %s\nsub.s %s, %s ,%d\ns.s %s, %s\n", reg, $3, reg, reg, $5.val.fval, reg, $1);
         freeRegisterF(reg);                                                    
      }
   }
      
   $$.head = strdup("");
   $$.body = strdup(codeBodyStr);
   free(codeBodyStr);
}
|ID ID ADDOP NUM
{
   outputError("expected an assigment operation");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|ID ASSIGNOP ADDOP NUM
{
   outputError("expected an ID");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|ID ASSIGNOP ID NUM
{
   outputError("expected an operation");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|ID ASSIGNOP ID ADDOP 
{
   outputError("expected a number");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|ID ASSIGNOP ID MULOP NUM
{//1   2    3     4      5
   printf("STEP : ID ASSIGNOP ID MULOP NUM\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   symTblEntry*  symbol1 = lookup(*ptr2symbolTable, $1);
   symTblEntry*  symbol2 = lookup(*ptr2symbolTable, $3);

   if (symbol1 == NULL || symbol2 == NULL)
   {
      outputError("Id not declared");
      hasErrors = true;
      $$.head = strdup("");
      $$.body = strdup("");
      free(codeBodyStr);
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
      if(symbol1->type == floating && symbol2->type == floating && $5.type == F)   // all floats assignment
      {
         char* reg = getRegisterF();
         if($4 == MUL)
            sprintf(codeBodyStr,"l.s, %s, %s\nmul.s %s, %s ,%d\ns.s %s, %s\n", reg ,$3, reg, reg, $5.val.fval, reg, $1);
         else
            sprintf(codeBodyStr,"l.s, %s, %s\ndiv.s %s, %s ,%d\ns.s %s, %s\n", reg, $3, reg, reg, $5.val.fval, reg, $1);
         freeRegisterF(reg);                                                    
      }
   }
   $$.head = strdup("");
   $$.body = strdup(codeBodyStr);
   free(codeBodyStr);
}
|ID ASSIGNOP MULOP NUM
{
   outputError("expected an ID");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
}
|ID ASSIGNOP ID MULOP
{
   outputError("expected a number");
   hasErrors = true;
   $$.body = strdup("");
   $$.head = strdup("");
};


BOOLEXPR : BOOLEXPR OROP BOOLTERM
{//0       1       2      3
   printf("BOOLEXPR --> BOOLEXPR OROP BOOLTERM\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* tmp;
   char* reg = getRegisterT();
   char* label = getLabel();

   sprintf(codeBodyStr,"bne %s, %s, else%s\nadd %s, $0, %s\nj end%s\nelse%s: addi %s, $0, 1\nend%s", $1.reg ,$3.reg, label, reg, $1.reg, label, label, reg, label);
   $$.reg = strdup(reg);
   tmp = strConcat($1.codeBody, $3.codeBody);
   $$.codeBody = strConcat(tmp, codeBodyStr);
   $$.codeHead = strConcat($1.codeHead, $3.codeHead);
   freeRegisterT($1.reg);
   freeRegisterT($3.reg);
   free(tmp);
   free(codeBodyStr);
}
|BOOLTERM
{//1
   printf("BOOLEXPR --> BOOLTERM\n");
   $$.reg = strdup($1.reg);
   $$.codeBody = strdup($1.codeBody);
   $$.codeHead = strdup($1.codeHead);
}
|BOOLEXPR BOOLTERM
{
   outputError("expected an OROP");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
};


BOOLTERM : BOOLTERM ANDOP BOOLFACTOR
{//0      1      2      3
   printf("BOOLTERM --> BOOLTERM ANDOP BOOLFACTOR\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* tmp;
   char* reg = getRegisterT();
   
   sprintf(codeBodyStr,"mul %s, %s, %s\n", reg, $1.reg ,$3.reg);
   $$.reg = strdup(reg);
   tmp = strConcat($1.codeBody, $3.codeBody);
   $$.codeBody = strConcat(tmp, codeBodyStr);
   $$.codeHead = strConcat($1.codeHead,$3.codeHead);
   freeRegisterT($1.reg);
   freeRegisterT($3.reg);
   free(tmp);
   free(codeBodyStr);
}
|BOOLTERM BOOLFACTOR
{
   outputError("expected an ANDOP");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|BOOLFACTOR
{//1
   printf("BOOLTERM --> BOOLFACTOR\n");
   $$.reg = strdup($1.reg);
   $$.codeBody = strdup($1.codeBody);
   $$.codeHead = strdup($1.codeHead);
};


BOOLFACTOR : '!''('BOOLFACTOR')'
{//0         1   2   3        4
   /*Meaning not BOOLFACTOR*/
   printf("BOOLFACTOR -->  '!' '(' BOOLFACTOR ')'\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* reg = getRegisterT();
   
   sprintf(codeBodyStr,"li %s ,1\nsub %s, %s, %s\n", reg, $3.reg, reg, $3.reg);
   $$.reg= $3.reg;
   $$.codeBody = strConcat($3.codeBody, codeBodyStr);
   $$.codeHead = $3.codeHead;
   freeRegisterT(reg);
   free(codeBodyStr);
}
|'!' BOOLFACTOR')'
{
   outputError("expected an '('");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|'!''(' ')'
{
   outputError("expected a BOOLFACTOR");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|'!''('BOOLFACTOR 
{
   outputError("expected a ')'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|EXPRESSION RELOP EXPRESSION
{//1         2      3
   printf("BOOLFACTOR -->  EXPRESSION  RELOP  EXPRESSION\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* tmpBody = (char*)calloc(200,sizeof(char));
   char* tmpHead = (char*)calloc(200,sizeof(char));
   char* reg = getRegisterT();

   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"int") == 0) //RELOP between ints
   {
      if($2 == EQ)
         sprintf(codeBodyStr,"seq %s, %s, %s\n", reg, $1.reg, $3.reg);
      if($2 == NEQ)
         sprintf(codeBodyStr,"sne %s, %s, %s\n", reg, $1.reg, $3.reg);
      if($2 == LT)
         sprintf(codeBodyStr,"slt %s, %s, %s\n", reg, $1.reg, $3.reg);
      if($2 == GT)
         sprintf(codeBodyStr,"sgt %s, %s, %s\n", reg, $1.reg, $3.reg);
      if($2 == LTEQ)
         sprintf(codeBodyStr,"sle %s, %s, %s\n", reg, $1.reg, $3.reg);
      if($2 == GTEQ)
         sprintf(codeBodyStr,"sge %s, %s, %s\n", reg, $1.reg, $3.reg);

      freeRegisterT($1.reg);
      freeRegisterT($3.reg);         
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"float") == 0) //RELOP between floats
   {
      char* label = getLabel();
      if($2 == EQ)
         sprintf(codeBodyStr,"c.eq.s %s, %s\nbc1f else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg ,$3.reg, label, reg, label, label, reg, label);
      if($2 == NEQ)
         sprintf(codeBodyStr,"c.eq.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg, $3.reg, label, reg, label, label, reg, label);
      if($2 == LT)
         sprintf(codeBodyStr,"c.lt.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $3.reg, $1.reg, label, reg, label, label, reg, label);
      if($2 == GT)
         sprintf(codeBodyStr,"c.lt.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg, $3.reg, label, reg, label, label, reg, label);
      if($2 == LTEQ)
         sprintf(codeBodyStr,"c.le.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $3.reg, $1.reg, label, reg, label, label, reg, label);
      if($2 == GTEQ)
         sprintf(codeBodyStr,"c.le.s %s, %s\nbc1t else%s\naddi %s,$0,1\nj end%s\nelse%s: move %s,$0\nend%s:\n", $1.reg, $3.reg, label, reg, label, label, reg, label);

      freeRegisterF($1.reg);
      freeRegisterF($3.reg);
   }

   $$.reg = strdup(reg);
   strcat(tmpBody,$1.codeBody);
   strcat(tmpBody,$3.codeBody);
   strcat(tmpBody,codeBodyStr);
   strcat(tmpHead,$1.codeHead);
   strcat(tmpHead,$3.codeHead);
   $$.codeBody = strdup(tmpBody);
   $$.codeHead = strdup(tmpHead);
   free(tmpBody);
   free(tmpHead);
   free(codeBodyStr);
};


EXPRESSION : EXPRESSION ADDOP TERM 
{//0         1         2      3
   printf("EXPRESSION --> EXPRESSION  ADDOP  TERM\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* tmp;
   
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"int") == 0) //ADDOP between ints
   {
      if($2 == PLUS)
         sprintf(codeBodyStr,"add %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      else
         sprintf(codeBodyStr,"sub %s, %s ,%s\n", $3.reg, $1.reg, $3.reg);
      freeRegisterT($1.reg);
      $$.type = strdup("int");
      $$.reg = strdup($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"int") == 0)   //ADDOP between a float and an int
   {
      char* reg = getRegisterF();
      if($2 == PLUS)
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nadd.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
      else
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nsub.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
      freeRegisterF(reg);
      freeRegisterT($3.reg);
      $$.type = strdup("float");
      $$.reg = strdup($1.reg);
   }
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"float") == 0)   //ADDOP between an int and a float
   {
      char* reg = getRegisterF();
      if($2 == PLUS)
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nadd.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, $3.reg, reg);
      else
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nsub.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, reg, $3.reg);
      freeRegisterF(reg);
      freeRegisterT($1.reg);
      $$.type = strdup("float");
      $$.reg = strdup($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"float") == 0)   //ADDOP between floats
   {
      if($2 == PLUS)
         sprintf(codeBodyStr,"add.s %s, %s ,%s\n", $3.reg, $1.reg, $3.reg);
      else
         sprintf(codeBodyStr,"sub.s %s, %s ,%s\n", $3.reg, $1.reg, $3.reg);
      freeRegisterF($1.reg);
      $$.type = strdup("float");
      $$.reg = strdup($3.reg);
   }         
   
   tmp = strConcat($1.codeBody, $3.codeBody);
   $$.codeBody= strConcat(tmp, codeBodyStr);
   $$.codeHead= strConcat($1.codeHead,$3.codeHead);
   free(tmp);
   free(codeBodyStr);
}
|TERM
{//1
   printf("EXPRESSION --> TERM\n");
   $$.reg =  strdup($1.reg);
   $$.codeBody = strdup($1.codeBody);
   $$.codeHead = strdup($1.codeHead);
   $$.type = strdup($1.type);
};


TERM : TERM MULOP FACTOR
{//0   1   2      3
   printf("TERM --> TERM MULOP FACTOR\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* tmp;
   
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"int") == 0)   //MULOP between ints
   {
      if($2 == MUL) 
         sprintf(codeBodyStr,"mul %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      else
         sprintf(codeBodyStr,"div %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      freeRegisterT($1.reg);
      $$.type = strdup("int");
      $$.reg = strdup($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"int") == 0) //MULOP between a float and an int
   {
      char* reg = getRegisterF();
      if($2 == MUL) 
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nmul.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
      else
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ndiv.s %s, %s ,%s\n", $3.reg, reg, reg, reg, $1.reg, $1.reg, reg);
      freeRegisterF(reg);
      freeRegisterT($3.reg);
      $$.type = strdup("float");
      $$.reg = strdup($1.reg);
   }
   if(strcmp($1.type,"int") == 0 && strcmp($3.type,"float") == 0)   //MULOP between an int and a float
   {
      char* reg = getRegisterF();
      if($2 == MUL) 
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\nmul.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, $3.reg, reg);
      else
         sprintf(codeBodyStr,"mtc1 %s, %s\ncvt.s.w %s, %s\ndiv.s %s, %s ,%s\n", $1.reg, reg, reg, reg, $3.reg, $3.reg, reg);
      freeRegisterF(reg);
      freeRegisterT($1.reg);
      $$.type = strdup("float");
      $$.reg = strdup($3.reg);
   }
   if(strcmp($1.type,"float") == 0 && strcmp($3.type,"float") == 0)   //MULOP between floats
   {
      if($2 == MUL)
         sprintf(codeBodyStr,"mul.s %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      else
         sprintf(codeBodyStr,"div.s %s, %s ,%s\n", $3.reg, $3.reg, $1.reg);
      freeRegisterF($1.reg);
      $$.type = strdup("float");
      $$.reg = strdup($3.reg);
   }
   
   tmp = strConcat($1.codeBody, $3.codeBody);
   $$.codeBody = strConcat(tmp, codeBodyStr);
   $$.codeHead = strConcat($1.codeHead, $3.codeHead);
   free(tmp);
   free(codeBodyStr);
}
|TERM FACTOR
{
   outputError("expected a MULOP");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|TERM MULOP
{
   outputError("expected a FACTOR");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|FACTOR
{//1
   printf("TERM --> FACTOR\n");
   $$.reg =  strdup($1.reg);
   $$.codeBody = strdup($1.codeBody);
   $$.codeHead = strdup($1.codeHead);
   $$.type = strdup($1.type);
};


FACTOR : '('EXPRESSION')'
{//0     1      2      3
   printf("FACTOR --> '(' EXPRESSION ')'\n");
   $$.reg =  strdup($2.reg);
   $$.codeBody = strdup($2.codeBody);
   $$.codeHead = strdup($2.codeHead);
   $$.type = strdup($2.type);
}
|'('EXPRESSION
{
   outputError("expected a ')'");
   hasErrors = true;
   $$.codeBody = strdup("");
   $$.codeHead = strdup("");
}
|ID
{//1
   printf("FACTOR --> ID\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* reg;
   char* label = getLabel();
   symTblEntry* symbol = lookup(*ptr2symbolTable, $1);
   
   if(symbol != NULL)
   {
      if(symbol->type == integer)
      {
         reg = getRegisterT();
         sprintf(codeBodyStr,"lw %s, %s\n", reg, $1);
      }
      if(symbol->type == floating)
      {
         reg = getRegisterF();
         sprintf(codeBodyStr,"l.s %s, %s\n", reg, $1);
      }
      if(symbol->type == string)
      {
         reg = $1;
      }
      $$.reg = strdup(reg);
      $$.type = EnumType2charType(symbol->type);
      $$.codeBody = strdup(codeBodyStr);
      $$.codeHead = strdup("");
   }
   else
   {
      outputError("Id not declared");
      hasErrors = true;
      $$.codeBody = strdup("");
      $$.codeHead = strdup("");
   }
   free(codeBodyStr);
}
|NUM
{//1
   printf("FACTOR --> NUM\n");
   char* codeBodyStr = (char*)calloc(200,sizeof(char));
   char* codeHeadStr = (char*)calloc(200,sizeof(char));
   char* reg;
   
   if($1.type == I) //int
   {
      reg = getRegisterT();
      sprintf(codeBodyStr,"addi %s, $0, %d\n", reg, $1.val.ival);
      $$.type = strdup("int");

   }
   else   //$1.type == F
   {
      reg = getRegisterF();
      char* label = getLabel();
      sprintf(codeHeadStr,"%s: .float %f\n", label, $1.val.fval);
      sprintf(codeBodyStr,"l.s %s, %s\n", reg, label);
      $$.type = strdup("float");
   }
   $$.reg = strdup(reg);
   $$.codeHead = strdup(codeHeadStr);
   $$.codeBody = strdup(codeBodyStr);
   free(codeHeadStr);
   free(codeBodyStr);
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

   printf("%s", mipsCode);

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
