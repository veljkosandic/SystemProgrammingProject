%{
  #include <cstdio>
  #include <iostream>
  #include <cstring>
  #include <string>
  #include <stdio.h>
  #include "../inc/asm/Assembly.hpp"
  extern int yylex();
  extern int yyparse();
  extern FILE* yyin;
  extern unsigned long line_num;
  const char* fname;
  void yyerror(const char *s);
  char* addOp(char* expr1,char* op,char* expr2){
    char* res = nullptr;
    unsigned long l1 = strlen(expr1),l2 = strlen(expr2),l=strlen(op);
                res = (char*)malloc((l1+l2+5)*sizeof(char));
                strcpy(res,expr1);res[l1]=' ';
                strcpy(res+l1+1,op);res[l1+l+1]=' ';
                strcpy(res + l1+l +2,expr2);
                //std::cout << res<< std::endl;
                //std::cout << expr1 << std::endl;
                free(expr1);
                free(expr2);
                free(op);
                return res;
}
std::string extractLine(unsigned long line){
  FILE* file = yyin;
  if(file!=NULL)fclose(file);
    char* l = NULL;
  unsigned long len = 0;
  file = fopen(fname,"r");
  for(unsigned long i=0;i<line-1;i++)
    getline(&l,&len,file);
  getline(&l,&len,file);
  fclose(file);
  file = NULL;
  return std::string(l);
}
%}
%code requires{
    struct opStruct{
    int opcode;
    char* sval;
  };
}
%union{
  long opcode;
  char* sval;
  struct opStruct str;
  char c;
}
%define parse.error verbose


%token <sval> SYMBOL
%token <sval> LITERRAL
%token <sval> GPR
%token <sval> CSR
%token <sval> LABEL
%token <sval> DOT
%token <sval> STRING
%token <opcode>HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR
%token <opcode>GLOBAL EXTERN SECTION WORD SKIP ASCII EQU END
%token <sval> LAND LOR EQ NEQ LEQ LESS GREAT GEQ PLUS MINUS OROP ANDOP XOROP ORNOT MULOP DIVOP MOD SHROP SHLOP NOTOP
%token SEP EXPRSTART EXPREND DOLLAR 
%token SUBEXPRSTART SUBEXPREND
%token ENDL 

%left LOR
%left LAND
%left EQ NEQ LEQ LESS GREAT GEQ
%left PLUS MINUS 
%left OROP ANDOP XOROP ORNOT
%left MULOP DIVOP MOD SHROP SHLOP
%left NOTOP


%type <str> dir globdir externdir sectiondir worddir skipdir asciidir equdir expr 
%type <sval> wlist numexpr gelist boperand operand wordelem
%type <opcode> NOINSTR OINSTR OGINSTR GGOINSTR GCINSTR GGINSTR GINSTR GOINSTR CGINSTR
%%
bison:
  lines{
    //std::cout << "Parsiranje zavrseno!" << std::endl;
  }
  ;
  lines:
    line | line lines | line endexpr;
    line:
      contents ENDL{line_num++;} | ENDL{line_num++;} 
      contents:
        LABEL{
          Assembly::Instance()->labelAnalysis($1);
          free($1);
        }; | LABEL expr{
          Assembly::Instance()->labelAnalysis($1);
          free($1);
          Assembly::Instance()->analyseOp($2.opcode,$2.sval);
          free($2.sval);
        }; | LABEL dir{
          Assembly::Instance()->labelAnalysis($1);
          free($1);
          Assembly::Instance()->analyseOp($2.opcode,$2.sval);
          free($2.sval);
        }; | expr{
          Assembly::Instance()->analyseOp($1.opcode,$1.sval);
          free($1.sval);
        }; | dir{
          Assembly::Instance()->analyseOp($1.opcode,$1.sval);
          free($1.sval);
        };
        dir:
          globdir{$$=$1;}; | externdir{$$=$1;}; | sectiondir{$$=$1;}; | worddir{$$=$1;}; | skipdir{$$=$1;}; | asciidir{$$=$1;}; | equdir{$$=$1;};
          globdir:
            GLOBAL gelist{$$.opcode=$1;$$.sval=$2;};
            gelist:
              SYMBOL{$$=$1;}  | gelist SEP SYMBOL{
                unsigned long l1 = strlen($1),l2 = strlen($3);
                $$ = (char*)malloc((l1+l2+2)*sizeof(char));
                strcpy($$,$1);$$[l1]=' ';strcpy($$ + l1 + 1,$3);
                free($1);
                free($3);
                //std::cout << $$ << std::endl;
              };
          externdir:
            EXTERN gelist{$$.opcode=$1;$$.sval=$2;};
          sectiondir:
            SECTION SYMBOL{$$.opcode=$1;$$.sval=$2;};
          worddir:
            WORD wlist{$$.opcode=$1;$$.sval=$2;};
            wlist:
              wordelem{$$=$1;} | wlist SEP wordelem{
                unsigned long l1 = strlen($1),l2 = strlen($3);
                $$ = (char*)malloc((l1+l2+2)*sizeof(char));
                strcpy($$,$1);$$[l1]=' ';strcpy($$ + l1 + 1,$3);
                free($1);
                free($3);
                //std::cout << $$ << std::endl;
              }; 
          wordelem: SYMBOL{$$=$1;} | LITERRAL{$$=$1;} | DOT {$$=$1;}
          skipdir:
            SKIP LITERRAL{$$.opcode=$1;$$.sval=$2;};
          asciidir:
            ASCII STRING{$$.opcode=$1;$$.sval=$2;};
          equdir:
            EQU SYMBOL SEP numexpr{$$.opcode=$1;
            unsigned long l1 = strlen($2),l2 = strlen($4);
            $$.sval=(char*)malloc(l1 + l2 + 2);
            char* x = $$.sval;
            strcpy(x,$2);x[l1]=' ';strcpy(x + l1 + 1,$4);
            free($2);free($4);
            };
              numexpr:
              LITERRAL{$$=$1;} | SYMBOL{$$=$1;} | DOT{$$=$1;}
              | SUBEXPRSTART numexpr SUBEXPREND{
                unsigned long l = strlen($2);
                $$ = (char*)malloc((l+5)*sizeof(char));
                $$[0]='(';$$[1]=' ';strcpy($$+2,$2);$$[l+3]=')';$$[l+2]=' ';$$[l+4]='\0';
                free($2);
                //std::cout << $$ << std::endl;
              };
              | MINUS numexpr %prec NOTOP{
                unsigned long l = strlen($2);
                $$ = (char*)malloc((l+4)*sizeof(char));
                $$[0]=' ';$$[1]='#';$$[2]=' ';strcpy($$+3,$2);
                free($2);
                free($1);
                //std::cout << $$ << std::endl;
              } ;
              | NOTOP numexpr %prec NOTOP{
                unsigned long l = strlen($2);
                $$ = (char*)malloc((l+4)*sizeof(char));
                $$[0]=' ';$$[1]='~';$$[2]=' ';strcpy($$+3,$2);
                free($2);
                free($1);
                //std::cout << $$ << std::endl;
              };
              | numexpr PLUS numexpr{$$=addOp($1,$2,$3);};
              | numexpr MINUS numexpr{$$=addOp($1,$2,$3);};
              | numexpr MULOP numexpr{$$=addOp($1,$2,$3);};
              | numexpr DIVOP numexpr{$$=addOp($1,$2,$3);};
              | numexpr MOD numexpr{$$=addOp($1,$2,$3);};
              | numexpr GEQ numexpr{$$=addOp($1,$2,$3);};
              | numexpr LEQ numexpr{$$=addOp($1,$2,$3);};
              | numexpr SHLOP numexpr{$$=addOp($1,$2,$3);};
              | numexpr SHROP numexpr{$$=addOp($1,$2,$3);};
              | numexpr LAND numexpr{$$=addOp($1,$2,$3);};
              | numexpr LOR numexpr{$$=addOp($1,$2,$3);};
              | numexpr OROP numexpr{$$=addOp($1,$2,$3);};
              | numexpr ANDOP numexpr{$$=addOp($1,$2,$3);};
              | numexpr XOROP numexpr{$$=addOp($1,$2,$3);};
              | numexpr ORNOT numexpr{$$=addOp($1,$2,$3);};
              | numexpr EQ numexpr{$$=addOp($1,$2,$3);};
              | numexpr NEQ numexpr{$$=addOp($1,$2,$3);};
              | numexpr LESS numexpr{$$=addOp($1,$2,$3);};
              | numexpr GREAT numexpr{$$=addOp($1,$2,$3);};              
        expr:
        NOINSTR{$$.opcode=$1;$$.sval=nullptr;}; | OINSTR operand{$$.opcode=$1;$$.sval=$2;}; |
        GGOINSTR GPR SEP GPR SEP boperand{
          $$.opcode=$1;
          unsigned long l1 = strlen($2),l2 = strlen($4),l3 = strlen($6);
          $$.sval = (char*)malloc((l1+l2+l3+3)*sizeof(char));
          strcpy($$.sval,$2);
          $$.sval[l1]=' ';
          strcpy($$.sval+l1+1,$4);
          $$.sval[l1 + l2 +1]=' ';
          strcpy($$.sval+l1+l2+2,$6);
          free($2);free($4);free($6);
        } | GINSTR GPR{
          $$.opcode=$1;$$.sval=$2;}; | 
        GGINSTR GPR SEP GPR{
                    $$.opcode=$1;
          unsigned long l1 = strlen($2),l2 = strlen($4);
          $$.sval = (char*)malloc((l1+l2+2)*sizeof(char));
          strcpy($$.sval,$2);
          $$.sval[l1]=' ';
          strcpy($$.sval+l1+1,$4);
          free($2);free($4);
        }; | OGINSTR operand SEP GPR{
                    $$.opcode=$1;
          unsigned long l1 = strlen($2),l2 = strlen($4);
          $$.sval = (char*)malloc((l1+l2+2)*sizeof(char));
          strcpy($$.sval,$2);
          $$.sval[l1]=' ';
          strcpy($$.sval+l1+1,$4);
          free($2);free($4);
        }; | GOINSTR GPR SEP operand{
                    $$.opcode=$1;
          unsigned long l1 = strlen($2),l2 = strlen($4);
          $$.sval = (char*)malloc((l1+l2+2)*sizeof(char));
          strcpy($$.sval,$2);
          $$.sval[l1]=' ';
          strcpy($$.sval+l1+1,$4);
          free($2);free($4);
        }; | CGINSTR CSR SEP GPR{
                    $$.opcode=$1;
          unsigned long l1 = strlen($2),l2 = strlen($4);
          $$.sval = (char*)malloc((l1+l2+2)*sizeof(char));
          strcpy($$.sval,$2);
          $$.sval[l1]=' ';
          strcpy($$.sval+l1+1,$4);
          free($2);free($4);
        }; | GCINSTR GPR SEP CSR{
                    $$.opcode=$1;
          unsigned long l1 = strlen($2),l2 = strlen($4);
          $$.sval = (char*)malloc((l1+l2+2)*sizeof(char));
          strcpy($$.sval,$2);
          $$.sval[l1]=' ';
          strcpy($$.sval+l1+1,$4);
          free($2);free($4);
        };
        boperand:
          LITERRAL{$$=$1;}; | SYMBOL {$$=$1;}; | DOT {$$ =$1;}
        operand://smije li razmak?
          DOLLAR LITERRAL {$$=(char*)malloc((strlen($2)+2)*sizeof(char));
          $$[0]='$';strcpy($$ + 1,$2);};|
           DOLLAR SYMBOL {$$=(char*)malloc((strlen($2)+2)*sizeof(char));
          $$[0]='$';strcpy($$ + 1,$2);free($2);};| 
          DOLLAR DOT{$$=(char*)malloc((strlen($2)+1)*sizeof(char));$$[0]='$';strcpy($$+1,$2);free($2);}|
          LITERRAL {$$=$1;}; |
           SYMBOL {$$=$1;};| DOT{$$=$1;} |GPR {$$=$1;}; |
           EXPRSTART GPR EXPREND{unsigned long l1=strlen($2);$$=(char*)malloc((l1+3)*sizeof(char));
            $$[0]='['; strcpy($$ +1,$2);$$[l1+1]=']';$$[l1+2]='\0';free($2);};
          | EXPRSTART GPR PLUS boperand EXPREND{unsigned long l1=strlen($2),l2 = strlen($4);
            $$=(char*)malloc((l1+l2+4)*sizeof(char));
            $$[0]='['; strcpy($$ +1,$2);$$[l1+1]='+';strcpy($$+l1+2,$4);$$[l1+l2+2]=']';$$[l1+l2+3]='\0';
            free($2);
            free($4);};
  NOINSTR :
    HALT{$$=$1;}; | INT{$$=$1;}; | IRET{$$=$1;}; |RET{$$=$1;};
  OINSTR:
    CALL{$$=$1;}; | JMP{$$=$1;};
  GGOINSTR:
  BEQ{$$=$1;}; | BNE{$$=$1;}; | BGT{$$=$1;};
  GINSTR:
  PUSH{$$=$1;}; | POP{$$=$1;}; | NOT{$$=$1;};
  GGINSTR:
  XCHG{$$=$1;}; | ADD{$$=$1;}; | SUB{$$=$1;}; | MUL{$$=$1;}; | DIV{$$=$1;}; | AND{$$=$1;}; | OR{$$=$1;}; | XOR{$$=$1;}; | SHL{$$=$1;}; | SHR{$$=$1;};
  OGINSTR:
  LD{$$=$1;};
  GOINSTR:
  ST{$$=$1;};
  CGINSTR:
  CSRRD{$$=$1;};
  GCINSTR:
  CSRWR{$$=$1;};
  endexpr:
    END{Assembly::Instance()->analyseOp($1,nullptr);}; | END ENDL{Assembly::Instance()->analyseOp($1,nullptr);};
%%

int lexicalAnalysis(const char* filename){
  FILE *fajl = fopen(filename,"r");
  fname = filename;
  //std::cout << src;
  if(!fajl){
    std::cout << "Greska u citanju fajla" << std::endl;
    exit(-1);
  }
  yyin = fajl;
  yyparse();
  fclose(fajl);
  yyin = NULL;
  return 0;
}

void yyerror(const char *s){
  std::cout << "Greska u leksickoj analizi fajla,linija " << line_num << " : " /*<< s*/ << std::endl;
  std::cout << extractLine(line_num) << std::endl;
  exit(-1);
}