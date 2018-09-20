#include "xconfig/driver.h"
#include "parser.hh"
#include "xconfig/scanner.h"
#include "xconfig/Mmvm.h"
#include <iostream>
#include <iostream>
using namespace std;
using namespace xconfig;

// ctor
comp_driver::comp_driver(shared_ptr<Mmvm>vm,ostream&os):
    trace_parsing_(false),trace_scanning_(true),vm_(vm),os_(os),haserror_(false){
}
// dtor
comp_driver::~comp_driver(){
}
// parse file
bool comp_driver::parse(istream&is,string const&streamname){
  // create scanner
  haserror_=false;
  streamname_=streamname;
  Scanner scanner(&is,&os_);
  scanner.set_debug(trace_scanning_);
  lexer_=&scanner;

  // parser
  yy::comp_parser parser(*this);
  parser.set_debug_level(trace_parsing_);
  return parser.parse()==0&&!haserror();
}
// trace related getters/setters
void comp_driver::trace_parsing(bool trace){trace_parsing_=trace;}
void comp_driver::trace_scanning(bool trace){trace_scanning_=trace;}
bool comp_driver::trace_parsing()const noexcept{return trace_parsing_;}
bool comp_driver::trace_scanning()const noexcept{return trace_scanning_;}

// error (with location)
void comp_driver::error(yy::location const&l,string const&m){
  haserror_=true;
  os_<<l<<": "<<m<<endl;
}
// error (no location specified)
void comp_driver::error(string const&m){
  haserror_=true;
  os_<<m<<endl;
}
// check if an error occured
bool comp_driver::haserror()const noexcept{
  return haserror_;
}
// get pointer to file
string&comp_driver::file()noexcept{return streamname_;}

// get current location
yy::location&comp_driver::loc(){
  return  loc_;
}
// get vm
shared_ptr<xconfig::Mmvm>comp_driver::vm(){return vm_;}

// get pointer to lexer object
Scanner*comp_driver::lexer()noexcept{return lexer_;}

// get symtab ref
xconfig::Symtab&comp_driver::symtab(){
  return symtab_;
}
