%{
#include "xconfig/scanner.h"              // scanner class
#include "xconfig/driver.h"               // driver class - we make callbacks to error functions in it
#include "xconfig/stringutils.h"
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include <algorithm>
using namespace std;

// import the parser's token type into a local typedef
typedef yy::comp_parser::token token;
typedef yy::comp_parser::token_type token_type;

// return correct type
#define yyterminate() return token::END

// easy access to location
#define loc driver.loc()
%}

%option noyywrap nounput batch debug noinput c++

/* add a prefix to scanner class*/
%option prefix="comp"

/* useful token definitions */
id      (%?[a-zA-Z][.a-zA-Z_0-9]*|%?\{[a-zA-Z][.a-zA-Z_0-9]*\})
env     (\$[a-zA-Z][a-zA-Z_0-9]*|\$\{[a-zA-Z][a-zA-Z_0-9]*\})
int     [0-9]+
blank   [ \t\r]
qstring \"(\\.|[^"\\])*\"
estring `(\\.|[^`\\])*\`

%{
/* run when a pattern is matched */
#define YY_USER_ACTION  loc.columns(yyleng);
%}

%%

%{
  // run when yylex is called
  loc.step();
%}

"#".*      loc.step();
{blank}+   loc.step(); 
[\n;]+     {
             string tmp=yytext;
             auto isnl=[](char c){return c=='\n';};
             int linecount=count_if(begin(tmp),end(tmp),isnl);
             loc.lines(linecount);loc.step();
             return yy::comp_parser::make_SEP(loc);
           }
"="        return yy::comp_parser::make_ASSIGN(loc); 
"+"        return yy::comp_parser::make_PLUS(loc); 
"`"        return yy::comp_parser::make_BS(loc); 
"("        return yy::comp_parser::make_LP(loc); 
")"        return yy::comp_parser::make_RP(loc); 
"{"        return yy::comp_parser::make_LB(loc); 
"}"        return yy::comp_parser::make_RB(loc); 
"@"        return yy::comp_parser::make_AT(loc); 
"namespace" return yy::comp_parser::make_NAMESPACE(loc);

{int}      {
             errno=0;
             long n=strtol(yytext,NULL,10);
             if(!(INT_MIN<=n&&n<=INT_MAX&&errno!=ERANGE)){
               driver.error(loc,"integer is out of range"); 
             }
             return yy::comp_parser::make_NUMBER(n,loc);
           }

{qstring}  { string tmp=yytext;
             tmp=tmp.substr(1,tmp.length()-2);
             auto res=xconfig::deescape(tmp,{'\"'});
             if(!res.first)driver.error(loc,res.second);
             return yy::comp_parser::make_QSTRING(res.second,loc);}

{estring}  { string tmp=yytext;
             tmp=tmp.substr(1,tmp.length()-2);
             auto res=xconfig::deescape(tmp,{'`'});
             if(!res.first)driver.error(loc,res.second);
             return yy::comp_parser::make_ESTRING(res.second,loc);}

{id}      {
             char*pyytext=yytext;
             if(yytext[0]=='%')++pyytext;
             string tmp=pyytext;
             if(tmp[0]=='{'){
               tmp=tmp.substr(1,tmp.length()-2);
             }
             return yy::comp_parser::make_IDENT(tmp,loc);
           }

{env}      {
             string tmp=yytext;
             if(tmp[1]=='{'){
               tmp=tmp.substr(2,tmp.length()-3);
             }else{
               tmp=tmp.substr(1);
             }
             return yy::comp_parser::make_ENV(tmp,loc);
           }
.          driver.error(loc,"invalid character");
<<EOF>>    return yy::comp_parser::make_END(loc);
%%
// -------------------- MORE CODE --------------------

// ctor
Scanner::Scanner(std::istream*arg_yyin,std::ostream*arg_yyout):compFlexLexer(arg_yyin,arg_yyout){
}

// dtor
Scanner::~Scanner(){
}
// enable debug output (via arg_yyout) if compiled into the scanner
void Scanner::set_debug(bool b){
  yy_flex_debug=b;
}

#ifdef yylex
#undef yylex
#endif

// must implement so we populate vtable
int compFlexLexer::yylex(){
  std::cerr<<"in compFlexLexer::yylex() ... should never happen"<<std::endl;
  return 0;
}
