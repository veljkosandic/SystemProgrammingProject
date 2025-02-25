%{
  #include <iostream>
  #include "bison.tab.h"
  unsigned long line_num = 1;
%}
literral ([0-9]{1,10})|(0x[0-9A-Fa-f]{1,8})|(0X[0-9A-Fa-f]{1,8})
gpregisters \%(r(([0-9])|(1[0-5]))|sp|pc)
csregisters \%(status|handler|cause)
%x COMM
%option noyywrap
%%
[ \t]          ;
\.global         {yylval.opcode=0;return GLOBAL;}
\.extern         {yylval.opcode=1;return EXTERN;}
\.section        {yylval.opcode=2;return SECTION;}
\.word           {yylval.opcode=3;return WORD;}
\.skip          {yylval.opcode=4;return SKIP;}
\.ascii         {yylval.opcode=5;return ASCII;}
\.equ           {yylval.opcode=6;return EQU;}
\.end           {yylval.opcode=7;return END;}
halt            {yylval.opcode=8;return HALT;}
int             {yylval.opcode=9;return INT;}
iret            {yylval.opcode=10;return IRET;}
call            {yylval.opcode=11;return CALL;}
ret             {yylval.opcode=12;return RET;}
jmp             {yylval.opcode=13;return JMP;}
beq             {yylval.opcode=14;return BEQ;}
bne             {yylval.opcode=15;return BNE;}
bgt             {yylval.opcode=16;return BGT;}
push            {yylval.opcode=17;return PUSH;}
pop             {yylval.opcode=18;return POP;}
xchg            {yylval.opcode=19;return XCHG;}
add             {yylval.opcode=20;return ADD;}
sub             {yylval.opcode=21;return SUB;}
mul             {yylval.opcode=22;return MUL;}
div             {yylval.opcode=23;return DIV;}
not             {yylval.opcode=24;return NOT;}
and             {yylval.opcode=25;return AND;}
or              {yylval.opcode=26;return OR;}
xor             {yylval.opcode=27;return XOR;}
shl             {yylval.opcode=28;return SHL;}
shr             {yylval.opcode=29;return SHR;}
ld              {yylval.opcode=30;return LD;}
st              {yylval.opcode=31;return ST;}
csrrd            {yylval.opcode=32;return CSRRD;}
csrwr            {yylval.opcode=33;return CSRWR;}
\"[^\n^\t]*\"          {yylval.sval = strdup(yytext);
                //std::cout << yytext << std::endl;
                return STRING;} 
^[\ \t]*[_a-zA-Z][a-zA-Z0-9_\.]*:    {yylval.sval = strdup(yytext);/*std::cout << "labela " << yytext << std::endl;*/
                      return LABEL;}
{gpregisters}           {yylval.sval=strdup(yytext);return GPR;}
{csregisters}           {yylval.sval=strdup(yytext);return CSR;}
{literral} {
                                yylval.sval=strdup(yytext);
                                /*std::cout <<"literal " <<  yytext << std::endl;*/
                                return LITERRAL;}
([_a-zA-Z][a-zA-Z0-9_\.]*)      {yylval.sval = strdup(yytext);//mora i tacka
                /*std::cout << "simbol " <<  yytext << std::endl;*/
                return SYMBOL;}
(".")           {yylval.sval = strdup(yytext);return DOT;}
#              {BEGIN(COMM);}
<COMM>.         ;
<COMM>\n       {BEGIN(INITIAL);return ENDL;}
,               {return SEP;}
\$              {return DOLLAR;}
\[              {return EXPRSTART;}
\]              {return EXPREND;}
\+              {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='+';yylval.sval[1]='\0';return PLUS;}
\-              {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='-';yylval.sval[1]='\0';return MINUS;}
\*              {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='*';yylval.sval[1]='\0';return MULOP;}
\/              {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='/';yylval.sval[1]='\0';return DIVOP;}
~               {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='~';yylval.sval[1]='\0';return NOTOP;}
%               {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='%';yylval.sval[1]='\0';return MOD;}
\>=              {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='>';yylval.sval[1]='=';yylval.sval[2]='\0';return GEQ;}
\<=              {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='<';yylval.sval[1]='=';yylval.sval[2]='\0';return LEQ;}  
\<\<              {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='<';yylval.sval[1]='<';yylval.sval[2]='\0';return SHLOP;}
\>\>              {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='>';yylval.sval[1]='>';yylval.sval[2]='\0';return SHROP;}
&&              {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='&';yylval.sval[1]='&';yylval.sval[2]='\0';return LAND;}
\|\|            {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='|';yylval.sval[1]='|';yylval.sval[2]='\0';return LOR;}
\|              {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='|';yylval.sval[1]='\0';return OROP;}
&               {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='&';yylval.sval[1]='\0';return ANDOP;}
\^              {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='^';yylval.sval[1]='\0';return XOROP;}
!               {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='!';yylval.sval[1]='\0';return ORNOT;}
==              {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='=';yylval.sval[1]='=';yylval.sval[2]='\0';return EQ;}
(!=)|(<>)       {yylval.sval = (char*)malloc(3*sizeof(char));yylval.sval[0]='!';yylval.sval[1]='=';yylval.sval[2]='\0';return NEQ;}
\<               {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='<';yylval.sval[1]='\0';return LESS;}
\>               {yylval.sval = (char*)malloc(2*sizeof(char));yylval.sval[0]='>';yylval.sval[1]='\0';return GREAT;}
\(              {return SUBEXPRSTART;}
\)              {return SUBEXPREND;}        
\n             {return ENDL;}
.               ;
%%