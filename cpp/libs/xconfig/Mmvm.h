// (C) Copyright Hans Ewetz 2018. All rights reserved.
#pragma once
#include "xconfig/MmvmError.h"
#include "xconfig/Symtab.h"
#include <string>
#include <iosfwd>
#include <vector>
#include <map>
#include <variant>
#include <functional>

// NOTE! TODO
/*
 * add:
 * (1) how to unify logging/tracing etc. across instructions - could we have info about #of expected operands, description etc. in instruction table
 * (2) code a simple debugger that displays all relevant data structures (stack, memory and program)
 */
namespace xconfig{

// vm class
class Mmvm{
public:
  // instructions
  enum class Opcode{
    stop=0,                          // stop program
    push_const=1,                    // push value stored below this instruction
    push_var=2,                      // push value of symbol stored in memory onto stack (symbol located below opcode)
    store_stack=3,                   // store top of stack to memory --> symbol (symbol name is after instruction)
    add_stack=4,                     // add two top elements on stack as strings and push result on stack
    push_env=5,                      // push value of environment variable onto stack (name of environment variabel stored below opcode)
    pop_stack=6,                     // pop stack
    shell=7,                         // execute program with arguments stored on stack and store output on stack
    interp=8,                        // interpolate string on stack and push result back in stack
    set_env=9,                       // store top of stack into environment variable following this opcode
    push_ns=10,                      // enter new namespace - ns specified as next memory locatino
    pop_ns=11,                       // enter new namespace
    add_sym=12                       // add symbol in current namespace
  };
  // typedefs
  using Value=std::variant<int,std::string>;           // data stored in symbol table, on stack or as operand in program
  using ProgElement=std::variant<Opcode,Value>;        // program consists of opcodes and values

  // ctor
  Mmvm();

  // generate code - returns address where code is located
  std::size_t code(Opcode);
  std::size_t code(Value const&val);
  std::size_t code(Opcode inst,Value const&val);

  // validate program
  MmvmError validatecode()const;

  // mem methods
  bool hassym(std::string const&name)const;
  std::optional<Value>getval(std::string const&name)const;
  void addsym(std::string const&name,Value const&val);

  // execution methods
  void run();

  // dump various pieces of information
  void dumpinst(std::ostream&os,Opcode i)const;
  void dumpvalue(std::ostream&os,Value const&v)const;
  void dumpprogelement(std::ostream&os,ProgElement const&p)const;

  // dump prog/stack/mem
  void dumpprog(std::ostream&os)const;
  void dumpstack(std::ostream&os)const;
  void dumpmem(std::ostream&os)const;
  void dumpsymtab(std::ostream&os)const;

  // symbol table methods
  // (used during string interpolation)
  xconfig::Symtab const&symtab()const noexcept;

  // get memory
  std::map<std::string,Value>const&mem()const;

  // value related methods
  std::string val2string(Value const&val)const;
private:
  // vm state
  std::size_t pc_;                      // program counter
  std::vector<ProgElement>prog_;        // program (opcodes and operands)
  std::vector<Value>stack_;             // stack
  std::map<std::string,Value>mem_;      // memory (addressed by symbol name)
  xconfig::Symtab symtab_;                // runtime symbol table - used during string interpolation

  // opcode --> instruction map
  struct Instr{
    Opcode opcode;                      // opcode
    std::size_t npargs;                 // #of operands for opcode
    std::string name;                   // name of opcode
    std::function<void(Mmvm*)>func;     // function executing the opcode
  };
  static std::map<Opcode,Instr>inst2info;

  // helper methods
  void popstack(std::size_t n2pop=1);
  void pushstack(Value const&v);
  Value const&stackval(size_t offset=0)const;
  size_t incpc();
  Instr const&nextinstr();
  Value const&nextprogval();
  std::pair<bool,std::string>getvar(std::string const&name)const;

  // instructions executing opcodes
  static void stop(Mmvm*);
  static void push_const(Mmvm*);
  static void push_var(Mmvm*);
  static void store_stack(Mmvm*);
  static void add_stack(Mmvm*);
  static void push_env(Mmvm*);
  static void pop_stack(Mmvm*);
  static void shell(Mmvm*);
  static void interp(Mmvm*);
  static void set_env(Mmvm*);
  static void push_ns(Mmvm*);
  static void pop_ns(Mmvm*);
  static void add_sym(Mmvm*);
};
}
