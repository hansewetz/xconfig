#include "xconfig/Mmvm.h"
#include "xconfig/MmvmError.h"
#include "xconfig/procutils.h"
#include "xconfig/stringutils.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <strstream>
#include <type_traits>
#include <cstdlib>
#include <cstring>
using namespace std;
namespace xconfig{

// helper functions
namespace{
// get environment variable
pair<bool,string>getenvvar(string const&envvar){
  char const*envval=std::getenv(envvar.c_str());
  if(!envval)return pair(false,"no environment variable named '"s+envvar+"'");
  return pair(true,string(envval));
}
// get environment variable
pair<bool,string>putenv(string const&envvar,string const&envval){
  int stat=setenv(envvar.c_str(),envval.c_str(),0);
  if(stat!=0){
    string errstr="failed setting environment variable: "s+envvar+" to value: '"+envval+"', error: "+strerror(errno);
    return pair(false,errstr);
  }
  return pair(true,string(envval));
}
// execute a cmd using a shell
pair<bool,string>execcmd(string const&cmd){
  string file="/usr/bin/bash";                 // NOTE! hardcoded - should be taken from a variable that can be set (i.e. SHELL)
  vector<string>args={"bash","-c"};
  args.push_back(cmd);
  return xconfig::execprog(file,args);
}
// convert a value to a string
string value2string(Mmvm::Value const&val){
  string ret;
  visit([&ret](auto const&v){
    stringstream str;
    str<<v;
    ret=str.str();
  },val);
  return ret;
}
// add two values
Mmvm::Value add2values(Mmvm::Value const&val1,Mmvm::Value const&val2){
  Mmvm::Value ret;
  visit([&ret,&val2](auto&&v1){
    using V1=std::decay_t<decltype(v1)>;
    if constexpr(std::is_same_v<V1,std::string>){  // val1 is string
      ret=value2string(v1)+value2string(val2);
    }else{ // V1 == int
      visit([&ret,&v1](auto&&v2){ 
        using V2=std::decay_t<decltype(v2)>;
        if constexpr(std::is_same_v<V2,int>){                                // val1 is int, val2 is int --> can sum them
          ret=v1+v2;
        }else{
          ret=value2string(v1)+value2string(v2);
        }
      },val2);
    }
  },val1);
  return ret;
}
}
// mapping from 'inst' --> string
map<Mmvm::Opcode,Mmvm::Instr>Mmvm::inst2info{
  {Mmvm::Opcode::stop,{Mmvm::Opcode::stop,0,"stop",Mmvm::stop}},
  {Mmvm::Opcode::push_const,{Mmvm::Opcode::push_const,1,"push_const",Mmvm::push_const}},
  {Mmvm::Opcode::push_var,{Mmvm::Opcode::push_var,1,"push_var",Mmvm::push_var}},
  {Mmvm::Opcode::store_stack,{Mmvm::Opcode::store_stack,1,"store_stack",Mmvm::store_stack}},
  {Mmvm::Opcode::add_stack,{Mmvm::Opcode::add_stack,0,"add_stack",Mmvm::add_stack}},
  {Mmvm::Opcode::push_env,{Mmvm::Opcode::push_env,1,"push_env",Mmvm::push_env}},
  {Mmvm::Opcode::pop_stack,{Mmvm::Opcode::pop_stack,0,"pop_stack",Mmvm::pop_stack}},
  {Mmvm::Opcode::shell,{Mmvm::Opcode::shell,0,"shell",Mmvm::shell}},
  {Mmvm::Opcode::interp,{Mmvm::Opcode::interp,0,"interp",Mmvm::interp}},
  {Mmvm::Opcode::set_env,{Mmvm::Opcode::set_env,1,"set_env",Mmvm::set_env}},
  {Mmvm::Opcode::push_ns,{Mmvm::Opcode::push_ns,1,"push_ns",Mmvm::push_ns}},
  {Mmvm::Opcode::pop_ns,{Mmvm::Opcode::pop_ns,0,"pop_ns",Mmvm::pop_ns}},
  {Mmvm::Opcode::add_sym,{Mmvm::Opcode::add_sym,0,"add_sym",Mmvm::add_sym}}
};
// ctor
Mmvm::Mmvm():pc_(0){
}
// add an instruction to program
size_t Mmvm::code(Opcode inst){
  prog_.push_back(inst);
  return prog_.size()-1;
}
// add an operand to program 
size_t Mmvm::code(Value const&val){
  prog_.push_back(val);
  return prog_.size()-1;
}
// add an instruction + operand to program 
size_t Mmvm::code(Opcode inst,Value const&val){
  code(inst);
  code(val);
  return prog_.size()-2;
}
// validate program
MmvmError Mmvm::validatecode()const{
  size_t addr=0;
  size_t ninstr=prog_.size();
  while(addr<ninstr){
    // get next program element and make sure it's an instruction
    ProgElement const&p=prog_[addr++];
    if(!holds_alternative<Opcode>(p)){   // we must have an opcode - or error
      string errstr="expected an opcode - found value '"+val2string(get<Value>(p))+"'";
      return MmvmError(addr-1,MmvmError::OPCODE_EXPECTED,errstr);
    }
    // get instruction
    Instr const&instr=inst2info[get<Opcode>(p)];
    if(addr+instr.npargs>ninstr){
      string errstr="opcode '"s+instr.name+"' requires "+std::to_string(instr.npargs)+" operands - the program text only has room for "+std::to_string(ninstr-addr-1);
      return MmvmError(addr-1,MmvmError::MISSING_OPERAND,errstr);
    }
    // loop through all operands and make sure they are all values
    for(size_t i=0;i<instr.npargs;++i){
      ProgElement const&p=prog_[addr++];
      if(!holds_alternative<Value>(p)){   // we must have a value - or error
        Instr const&instr=inst2info[get<Opcode>(p)];
        string errstr="expected a value - found opcode '"+instr.name+"'";
        return MmvmError(addr-1,MmvmError::OPCODE_EXPECTED,errstr);
      }
    }
  }
  return MmvmError(addr,MmvmError::OK,"");
}
// check if a symbol exists
bool Mmvm::hassym(string const&name)const{
  return mem_.count(name);
}
// get value from memory of a symbol
optional<Mmvm::Value>Mmvm::getval(string const&name)const{
  if(hassym(name))return mem_.find(name)->second;
  return optional<Value>{};
}
// add a symbol with value to symbol table
void Mmvm::addsym(string const&name,Value const&val){
  if(hassym(name)){
    throw MmvmError(pc_,MmvmError::SYM_EXISTS,"attempt to add existing symbol '"s+name+"' to mem");
  }
  mem_[name]=val;
}
// run program
void Mmvm::run(){
  if(prog_.size()==0)return;
  while(true){
    Instr const&instr=nextinstr();
    instr.func(this);
    if(instr.opcode==Mmvm::Opcode::stop)break;
  }
}
// dump an instruction
void Mmvm::dumpinst(ostream&os,Opcode i)const{
  cout<<inst2info[i].name;
}
// dump a value
void Mmvm::dumpvalue(ostream&os,Value const&v)const{
  visit([&os](auto&&arg){
          os<<arg<<"["<<typeid(arg).name()<<"]";
        },v);
}
// dump a single program element
void Mmvm::dumpprogelement(ostream&os,ProgElement const&p)const{
  visit([this,&os](auto&&el){
         using T=std::decay_t<decltype(el)>;
         if constexpr(std::is_same_v<T,Opcode>){
           dumpinst(os,el);
         }else{
           dumpvalue(os,el);
         }
        },p);
}
// dump program in readable form
void Mmvm::dumpprog(ostream&os)const{
  size_t no=0;
  for(auto const&p:prog_){
    os<<setfill('0')<<setw(5)<<no++<<": ";
    dumpprogelement(os,p);
    os<<endl;
  }
}
// dump stack
void Mmvm::dumpstack(std::ostream&os)const{
  int no=0;
  for(auto i=stack_.size();i>0;--i){
    os<<setfill('0')<<setw(5)<<no++<<": ";
    dumpvalue(os,stack_[i-1]);
    os<<endl;
  }
}
// dump symbol table
void Mmvm::dumpmem(ostream&os)const{
  for(auto const&p:mem_){
    os<<p.first<<"-->";
    dumpvalue(os,p.second);
    os<<endl;
  }
}
// dump symbol table
void Mmvm::dumpsymtab(std::ostream&os)const{
  os<<symtab_<<endl;
}
// get memory
map<string,Mmvm::Value>const&Mmvm::mem()const{
  return mem_;
}
// ---------------- symbol table methods
xconfig::Symtab const&Mmvm::symtab()const noexcept{
  return symtab_;
}
// ---------------- value raletd methods
// convert a value to a string
string Mmvm::val2string(Mmvm::Value const&val)const{
  return value2string(val);
}
// ---------------- helper methods
void Mmvm::popstack(size_t n2pop){   // pop a number of elements off the stack
  for(size_t i=0;i<n2pop;++i)stack_.pop_back();
}
void Mmvm::pushstack(Value const&v){ // push an element on stack
  stack_.push_back(v);
}
Mmvm::Value const&Mmvm::stackval(size_t offset)const{
  return stack_[stack_.size()-1-offset];
}
size_t Mmvm::incpc(){
  return pc_++;
}
Mmvm::Instr const&Mmvm::nextinstr(){
  Opcode inst=get<Opcode>(prog_[incpc()]);
  return inst2info[inst];
}
Mmvm::Value const&Mmvm::nextprogval(){    // get next value from program memory
  return get<Value>(prog_[incpc()]);
}
pair<bool,string>Mmvm::getvar(string const&name)const{
  if(!mem_.count(name))return pair(false,"no variable named '"s+name+"'");
  return pair(true,val2string(mem_.find(name)->second));
}
// ---------------- instructions
void Mmvm::stop(Mmvm*vm){   // stop - dummy instruction
  vm->incpc();
}
void Mmvm::push_const(Mmvm*vm){  // push value below 'pc'
  Value const&val=vm->nextprogval();
  vm->pushstack(val);
}
void Mmvm::push_var(Mmvm*vm){  // push value of symbol having name located below pc
  string const&symname=get<string>(vm->nextprogval());
  if(!vm->hassym(symname))throw MmvmError(vm->pc_,MmvmError::NO_SYM,"no symbol named: "s+symname,"operation 'pushs'");
  vm->pushstack(vm->mem_[symname]);
}
void Mmvm::store_stack(Mmvm*vm){  // store top of stack --> symbol (symbol name is after instruction)
  string const&symname=get<string>(vm->nextprogval());
  Value const&val=vm->stackval();
  vm->mem_[symname]=val;
}
void Mmvm::add_stack(Mmvm*vm){  // add two top elements on stack as strings and push result on stack
  Value res=add2values(vm->stackval(1),vm->stackval(0));
  vm->popstack(2);
  vm->pushstack(res);
}
void Mmvm::push_env(Mmvm*vm){  // push value of environment variable onto stack (name of environment variabel stored below opcode)
  Value const&val=vm->nextprogval();
  if(!holds_alternative<string>(val)){   // we must have a string - or error
    string errstr="invalid operand found";
    string detail="expected string as operand to 'push_env' - found value '"+vm->val2string(val)+"'";
    throw MmvmError(vm->pc_,MmvmError::EXPECT_STRING,errstr,detail);
  }
  // get environment variable
  auto envres=getenvvar(get<string>(val));
  if(!envres.first)throw MmvmError(vm->pc_,MmvmError::NOSUCH_ENVVAR,envres.second,"operation 'pushe'");
  vm->pushstack(envres.second);
}
void Mmvm::pop_stack(Mmvm*vm){  // pop_stack
  vm->popstack(1);
}
void Mmvm::shell(Mmvm*vm){  // execute program, store output on stack
  string execstr=vm->val2string(vm->stackval());
  auto[err,res]=execcmd(execstr);
  if(!err)throw MmvmError(vm->pc_,MmvmError::SHELL_ERROR,res,"operation 'shell'");
  vm->popstack(1);
  vm->pushstack(res);
}
void Mmvm::interp(Mmvm*vm){  // interpolate string on stack and push result back in stack
  string str=vm->val2string(vm->stackval());
  auto fgetvar=[vm](string const&name){return vm->getvar(name);};
  auto res=xconfig::interpolate(str,getenvvar,fgetvar,execcmd,vm->symtab());
  if(!res.first){
    throw MmvmError(vm->pc_,MmvmError::INTERP_ERROR,"string interpolation error",res.second);
  }
  vm->popstack(1);
  vm->pushstack(res.second);
}
void Mmvm::set_env(Mmvm*vm){  // store top of stack in environment variable following this opcode
  Value const&envvar=vm->nextprogval();
  if(!holds_alternative<string>(envvar)){   // we must have a string - or error
    string errstr="invalid operand found";
    string detail="expected string as operand to 'set_env' - found value '"+vm->val2string(envvar)+"'";
    throw MmvmError(vm->pc_,MmvmError::EXPECT_STRING,errstr,detail);
  }
  // get top of stack
  auto const&envval=vm->stackval();

  // set environment variable
  auto envres=putenv(get<string>(envvar),vm->val2string(envval));
  if(!envres.first)throw MmvmError(vm->pc_,MmvmError::NOSUCH_ENVVAR,envres.second,"operation 'set_env'");
}
void Mmvm::push_ns(Mmvm*vm){
  Value const&ns=vm->nextprogval();
  if(!holds_alternative<string>(ns)){   // we must have a string - or error
    string errstr="invalid operand found";
    string detail="expected string as operand to 'push_ns' - found value '"+vm->val2string(ns)+"'";
    throw MmvmError(vm->pc_,MmvmError::EXPECT_STRING,errstr,detail);
  }
  vm->symtab_.pushns(get<string>(ns));
}
void Mmvm::pop_ns(Mmvm*vm){
  vm->symtab_.popns();
}
void Mmvm::add_sym(Mmvm*vm){
  Value const&sym=vm->nextprogval();
  if(!holds_alternative<string>(sym)){   // we must have a string - or error
    string errstr="invalid operand found";
    string detail="expected string as operand to 'add_sym' - found value '"+vm->val2string(sym)+"'";
    throw MmvmError(vm->pc_,MmvmError::EXPECT_STRING,errstr,detail);
  }
  vm->symtab_.addsym(get<string>(sym));
}
}
