%skeleton "lalr1.cc"                         // use newer c++ skeleton file
%require "3.0.4"                             // minimum version of bison
%defines                                     // write header containing token defines
%define parser_class_name {comp_parser}      // set name of parser class

// use c++ objects as values
%define api.token.constructor
%define api.value.type variant
%define parse.assert

// includes and decl. for generated 'parser.hh'
%code requires{
#include <string>                              // need string header
class comp_driver;                             // forward decl of driver
}

// parameters passed to parser ctor
%param{comp_driver&driver}

// track location (file,line,col --> location.hh, position.hh)
// (initialize location to filename from driver)
%locations
%initial-action{
  // initial location
  @$.begin.filename=@$.end.filename=&driver.file();
};

// enable parser tracing and verbose error messages
%define parse.trace
%define parse.error verbose

// includes for generated 'parser.cc'
%code{
#include "xconfig/driver.h"
#include "xconfig/scanner.h"
#include "xconfig/Mmvm.h"
#include <iostream>
#include <optional>
using namespace std;

// easy access to mmv stuff
using namespace xconfig;
using op=xconfig::Mmvm::Opcode;

// easy access to symtab
#define symtab driver.symtab()

// connect bison parser --> flex scanner via driver
#undef yylex
#define yylex driver.lexer()->lex

// access to vm
// (makes code generation less noisy)
#define vm (*driver.vm())

// easy access to location
#define loc driver.loc()
}

// explicit token numbering
%define api.token.prefix {TOK_}
%token
  END  0  "end of file"
  ASSIGN  "="
  PLUS    "plus"
  SEP     "sep"
  BS      "backslash"
  LP      "left paren"
  RP      "right paren"
  AT      "at sign"
  LB      "left brace"
  RB      "right brace"
  NAMESPACE      "namespace"
;

// semantic values are c++ objects (i.e. variant based)
%token <std::string> IDENT "identifier"
%token <std::string>  QSTRING "quoted string"
%token <std::string>  ESTRING "execution string"
%token <std::string>  EXEC "exec"
%token <std::string> ENV "environment-variable"
%token <int> NUMBER "number"

// grammar
%%
prog: stmts              {vm.code(op::stop);}
    ;
stmts:
    | stmts stmt
    ;
stmt: SEP
    | expr SEP            {vm.code(op::pop_stack);}
    | nsdecl LB stmts RB  {symtab.popns();vm.code(op::pop_ns);}
    ;
nsdecl: NAMESPACE IDENT   {if(!symtab.isSimpleSymbol($2)){
                             error(loc,"invalid namespace identifier: '"s+$2+"' (contains '.')");
                             YYERROR;
                           }
                           symtab.pushns($2);vm.code(op::push_ns,$2);}
    ;
expr: value
    | expr PLUS expr      {vm.code(op::add_stack);}
    | BS expr BS          {vm.code(op::shell);}
    | IDENT ASSIGN expr   {if(!symtab.isSimpleSymbol($1)){
                             error(loc,"cannot assign to namespace qualified symbol: '"s+$1+"'");
                             YYERROR;
                           }
                           if(symtab.lookupsym(symtab.fullyQualifiedName($1))){
                             error(loc,"symbol: '"s+$1+"' already exist in current namespace");
                             YYERROR;
                           }
                           auto sym=symtab.addsym($1);
                           vm.code(op::add_sym,$1);        // code non-qualified name in symbol table
                           vm.code(op::store_stack,sym);   // code fully qualified name as memory address
                          }
    | ENV ASSIGN expr     {vm.code(op::set_env,$1);}       // will store top of stack in environment variable $1
    | AT expr             {vm.code(op::interp);}
    | LP expr RP 
    ;
value: NUMBER  {vm.code(op::push_const,$1);}
     | QSTRING {vm.code(op::push_const,$1);}
     | ESTRING {vm.code(op::push_const,$1);vm.code(op::shell);}
     | ENV     {vm.code(op::push_env,$1);}
     | IDENT   {auto sym=symtab.lookupsym($1);
                if(!sym.has_value()){
                  error(loc,"no such symbol in current or enclosing namespaces: '"s+$1+"'");
                  YYERROR;
                }
                vm.code(op::push_var,sym.value());}
     ;
%%

// error function - forward errors to driver
void yy::comp_parser::error(const location_type&l,const std::string&m){
  driver.error(l,m);
}
