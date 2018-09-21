// (C) Copyright Hans Ewetz 2018. All rights reserved.
#include "xconfig/MmvmError.h"
#include <iostream>
#include <sstream>
using namespace std;
namespace xconfig{
ostream&operator<<(ostream&os,MmvmError const&e){
  return os<<"addr: "<<e.addr_<<", errc: "<<static_cast<int>(e.errcd_)<<", errstr: "<<e.errstr_<<", detail: "<<e.detail_;
}
// ctor
MmvmError::MmvmError(size_t addr,MmvmError::error errcd,string const&errstr,string const&detail):
    runtime_error(errstr.c_str()),addr_(addr),errcd_(errcd),errstr_(errstr),detail_(detail){
  stringstream str;
  str<<*this;
  fullerr_=str.str();
}
MmvmError::MmvmError(size_t addr,MmvmError::error errcd,string const&errstr):MmvmError(addr,errcd,errstr,""){}
MmvmError::MmvmError(size_t addr,MmvmError::error errcd):MmvmError(addr,errcd,"",""){}
// getters
MmvmError::operator bool()const noexcept{return errcd_==OK;}
size_t MmvmError::addr()const noexcept{return addr_;}
MmvmError::error MmvmError::errcode()const noexcept{return errcd_;}
const char*MmvmError::what()const noexcept{return fullerr_.c_str();}
string const&MmvmError::tostring()const{return fullerr_;}
}
