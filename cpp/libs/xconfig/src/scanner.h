#pragma once

// forward decl - used in 'lex()' function
class driver;

// signature of yylex must be visible to flex
#ifndef YY_DECL
#define YY_DECL yy::comp_parser::symbol_type Scanner::lex(comp_driver&driver)
#endif

// 'yyFlexLexer' must be defined in FlexLexer.h
#ifndef __FLEX_LEXER_H
#define yyFlexLexer compFlexLexer
#include "FlexLexer.h"
#undef yyFlexLexer
#endif

// include generated header for parser
// (need it for 'yy::comp_parser::symbol_type)
#include "parser.hh"

// scanner class - implemented in baseclass FlexLexer (copied from flex distr)
class Scanner:public compFlexLexer{
public:
    // ctor/dtor
    Scanner(std::istream*arg_yyin,std::ostream*arg_yyout);
    virtual~Scanner();

    // scanner function - we take the driver as parameter
    virtual yy::comp_parser::symbol_type lex(comp_driver&driver);

    // enable debug output (via arg_yyout) if compiled into the scanner
    void set_debug(bool b);
};
