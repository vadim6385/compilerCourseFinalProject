%{
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

int col = 1;
int line = 1;
%}
%option noyywrap yylineno
DIGIT [0-9]
LETTER [a-zA-Z]
%x comment
%%

"/*"	BEGIN(comment);
<comment>[^*]*
<comment>"*/" BEGIN(0);
[\n]		{ line++; col = 1; }
[\t]		{ col += 4;}
[" "]		{ col += 1;}
"start"		{ col += yyleng; return START;}
"break"		{ col += yyleng; return BREAK;}
"case"		{ col += yyleng; return CASE;} 
"final"		{ col += yyleng; return FINAL;}
"default"	{ col += yyleng; return DEFAULT;}
"do"		{ col += yyleng; return DO;}
"else"		{ col += yyleng; return ELSE;}
"end"		{ col += yyleng; return END;}
"foreach"	{ col += yyleng; return FOREACH;}
"if"		{ col += yyleng; return IF;}
"int"		{ col += yyleng; return INT;}
"decl"		{ col += yyleng; return DCL;}
"out"		{ col += yyleng; return OUT;}
"program"	{ col += yyleng; return PROGRAM;}
"real"		{ col += yyleng; return REAL;}
"read"		{ col += yyleng; return READ;}
"string"	{ col += yyleng; return STRING;}
"switch"	{ col += yyleng; return SWITCH;}
"till"		{ col += yyleng; return TILL;}
"var"		{ col += yyleng; return VAR;}
"while"		{ col += yyleng; return WHILE;}
"with"		{ col += yyleng; return WITH;}

")"		{ col += yyleng; return ')';}
"("		{ col += yyleng; return '(';}
"{"		{ col += yyleng; return '{';}
"}"		{ col += yyleng; return '}';}
","		{ col += yyleng; return ',';}
":"		{ col += yyleng; return ':';}
";"		{ col += yyleng; return ';';}
"!"		{ col += yyleng; return '!';}

{LETTER}({LETTER}|{DIGIT}){0,8} {
col += yyleng;
yylval.sval = (char*)malloc(yyleng*sizeof(char) +1);
strcpy(yylval.sval, yytext);

return ID;}

{DIGIT}+			{ col += yyleng;
				 fprintf(yyout, "%s", yytext);
				 yylval.nVal.val.ival = atoi(yytext); 
				 yylval.nVal.type = I;    
				 return NUM;}

{DIGIT}+.{DIGIT}*		{ col += yyleng;
				 yylval.nVal.val.fval = atof(yytext); 
				 yylval.nVal.type = F;
				 return NUM;}

\"({LETTER}|"."|","|"!"|"?"|" "|{DIGIT})*\" {
col += yyleng;
yylval.sval = (char*)malloc(yyleng*sizeof(char) +1);
strcpy(yylval.sval, yytext);
return SENTENCE;}

"+"			{ col += yyleng; yylval.op = PLUS; return ADDOP;}
"-"			{ col += yyleng; yylval.op = MINUS; return ADDOP;}
"=="			{ col += yyleng; yylval.op = EQ; return RELOP;}
"<>"			{ col += yyleng; yylval.op = NEQ; return RELOP;}
"<"			{ col += yyleng; yylval.op = LT; return RELOP;}
">"			{ col += yyleng; yylval.op = GT; return RELOP;}
">="			{ col += yyleng; yylval.op = GTEQ; return RELOP;}
"<="			{ col += yyleng; yylval.op = LTEQ; return RELOP;}
"*"			{ col += yyleng; yylval.op = MUL; return MULOP;}
"/"			{ col += yyleng; yylval.op = DIV; return MULOP;}
"="			{ col += yyleng; yylval.op = ASSIGN; return ASSIGNOP;}
"||"			{ col += yyleng; yylval.op = OR; return OROP;}
"&&"			{ col += yyleng; yylval.op = AND; return ANDOP;}
%%