// (C) Copyright Hans Ewetz 2018. All rights reserved.
#pragma once
#include <stdexcept>
#include <string>
#include <iosfwd>
namespace xconfig{

// mmvm exception
class MmvmError:public std::runtime_error{
public:
  // debug print function
  friend std::ostream&operator<<(std::ostream&os,MmvmError const&e);

  // error codes
  enum error{
    OK=0,                                // no error
    NO_SYM,                              // symbol does not exist in symbol table
    SYM_EXISTS,                          // symbol already exists in symbol table
    OPCODE_EXPECTED,                     // program slot contains a value but should contain an opcode
    MISSING_OPERAND,                     // missing operand(s) after opcode
    EXPECT_STRING,                       // expected string as operand
    NOSUCH_ENVVAR,                       // environment variable not set in environment
    SHELL_ERROR,                         // error while executing an external program using the shell
    INTERP_ERROR                         // error while interpolating string
  };
  // ctor,assign,dtor
  MmvmError(std::size_t addr,error errcd);
  MmvmError(std::size_t addr,error errcd,std::string const&errstr);
  MmvmError(std::size_t addr,error errcd,std::string const&errstr,std::string const&detail);

  // getters
  operator bool()const noexcept;
  std::size_t addr()const noexcept;
  error errcode()const noexcept;
  virtual const char*what()const noexcept;
  std::string const&tostring()const;
private:
  std::size_t addr_;
  error errcd_;
  std::string errstr_;
  std::string detail_;
  std::string fullerr_;
};
}
