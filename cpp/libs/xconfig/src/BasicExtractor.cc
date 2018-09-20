#include "xconfig/BasicExtractor.h"
#include "xconfig/Mmvm.h"
using namespace std;
namespace xconfig{

// ctor
BasicExtractor::BasicExtractor(shared_ptr<Mmvm>vm):Extractor(vm){
}
// get all variables/values as string
map<string,string>BasicExtractor::operator()()const{
  map<string,string>ret;
  auto const&mem=vm()->mem();
  for(auto&&[name,value]:mem){
    ret.insert(pair(name,vm()->val2string(value)));
  }
  return ret;
}
// get a variable as a string by name
optional<string>BasicExtractor::operator()(string const&name)const{
  auto res=vm()->getval(name);
  if(res)return vm()->val2string(res.value());
  return optional<string>{};
}
// get values from a vector of names
map<string,optional<string>>BasicExtractor::operator()(vector<string>const&v)const{
  map<string,optional<string>>ret;
  for(auto const&s:v)ret[s]=(*this)(s);
  return ret;
}
// get values for variables having names matching a regular expression
map<string,string>BasicExtractor::operator()(std::regex const&r)const{
  map<string,string>ret;
  auto const&mem=vm()->mem();
  for(auto&&[name,value]:mem){
    if(!regex_match(name,r))continue;
    ret.insert(pair(name,vm()->val2string(value)));
  }
  return ret;
}
map<string,string>BasicExtractor::regex(string const&rstr)const{
  return operator()(std::regex(rstr));
}
// get variables/values for a specific namespace
map<string,string>BasicExtractor::ns(string const&ns)const{
  std::regex r{ns+"\\.[a-zA-Z0-9_\\.]+"};
  return operator()(r);
}
map<string,string>BasicExtractor::ns(vector<string>const&nss)const{
  map<string,string>ret;
  for(auto&&n:nss){
    for(auto&&[name,value]:ns(n))ret[name]=value;
  }
  return ret;
}
// NOTE! testing
map<string,Mmvm::Value>BasicExtractor::asValue()const{
  return vm()->mem();
}
optional<Mmvm::Value>BasicExtractor::asValue(string const&name)const{
  return vm()->getval(name);
}
}
