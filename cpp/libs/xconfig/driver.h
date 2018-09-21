// (C) Copyright Hans Ewetz 2018. All rights reserved.
#pragma once
#include "parser.hh"   // needed for 'yy::location'
#include "xconfig/Symtab.h"
#include <string>
#include <memory>

// forward decl
namespace xconfig{class Mmvm;}
class Scanner;

// Conducting the whole scanning and parsing of comp.
class comp_driver{
public:
  // ctors,dtor,assign
  comp_driver(std::shared_ptr<xconfig::Mmvm>vm,std::ostream&os);
  comp_driver(comp_driver const&)=delete;
  comp_driver(comp_driver&&)=delete;
  comp_driver&operator=(comp_driver const&)=delete;
  comp_driver&operator=(comp_driver&&)=delete;
  virtual~comp_driver();

  // parse file
  // (return true on success)
  bool parse(std::istream&is,const std::string&streamname);

  // tracing related functions
  // (getters/setters)
  void trace_parsing(bool trace);
  void trace_scanning(bool trace);
  bool trace_parsing()const noexcept;
  bool trace_scanning()const noexcept;

  // error functions
  void error(const yy::location& l,std::string const&m);
  void error(const std::string& m);
  bool haserror()const noexcept;

  // file being parsed
  // (used later to pass the file name to the location tracker)
  std::string&file()noexcept;

  // get current location
  yy::location&loc();

  // get vm
  std::shared_ptr<xconfig::Mmvm>vm();

  // get pointer to lexer object
  Scanner*lexer()noexcept;

  // get symtab ref
  xconfig::Symtab&symtab();
private:
  // whether parser traces should be generated
  bool trace_parsing_;
  bool trace_scanning_;
  std::string streamname_;
  Scanner*lexer_;
  xconfig::Symtab symtab_;
  std::shared_ptr<xconfig::Mmvm>vm_;
  std::ostream&os_;
  bool haserror_;
  yy::location loc_;
};
