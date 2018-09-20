#include "xconfig/Symtab.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;
namespace xconfig{

// debug print method
ostream&operator<<(ostream&os,Symtab const&st){
  os<<">>>namespace-stack: "<<endl;
  for(auto it=begin(st.nsstack_);it!=end(st.nsstack_);++it){
    os<<*it;
    if(next(it)!=end(st.nsstack_))os<<".";
  }
  cout<<endl;
  os<<">>>namespace-table: "<<endl;
  for(auto const&ns:st.nstab_)cout<<"\t"<<ns<<endl;
  os<<">>>symbol-table: "<<endl;
  for(auto const&sym:st.symtab_)cout<<"\t"<<sym<<endl;
  return os;
}
// ns management
void Symtab::pushns(string const&name){
  // note: we allow to open up a ns that already exist
  if(nsstack_.empty())nsstack_.push_back(name);
  else nsstack_.push_back(nsstack_.back()+NSSEP+name);
}
void Symtab::popns(){
  string ns=nsstack_.back();
  nsstack_.pop_back();
}
string Symtab::currentns()const noexcept{
  return nsstack_.empty()?""s:nsstack_.back();
}
// symbol management
optional<string>Symtab::lookupsym(string const&name)const{
  for(size_t i=0;i<=nsstack_.size();++i){
    string fqname;
    if(i==nsstack_.size())fqname=name;
    else fqname=nsstack_[nsstack_.size()-i-1]+NSSEP+name;
    if(symtab_.count(fqname))return fqname;
  }
  return optional<string>{};
}
string Symtab::addsym(string const&name){
  string fqname=fullyQualifiedName(name);
  if(symtab_.count(fqname)){
    stringstream str;
    str<<"<internal compilation error>attempt to insert symbol: "<<name<<" - name already exists at current level"<<endl<<
                    "symtab dump: "<<endl<<*this;
    throw runtime_error(str.str());
  }
  symtab_.insert(fqname);
  return fqname;
}
bool Symtab::isSimpleSymbol(std::string const&name)const{
  return name.find(NSSEP)==string::npos;
}
string Symtab::fullyQualifiedName(std::string const&name)const{
  return nsstack_.empty()?name:currentns()+NSSEP+name;
}
}
