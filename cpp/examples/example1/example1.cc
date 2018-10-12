// (C) Copyright Hans Ewetz 2018. All rights reserved.
#include "xconfig/XConfig.h"
#include <iostream>
#include <strstream>
using namespace std;
using namespace xconfig;

/*
 * simple example demonstrating:
 * - read config file
 * - print all variables
 * - print all variables inside a namespace
 * - print all variables matching a regular expression
 */
int main(){
  try{
    XConfig xfg("example1.cfg");

    // get all variables
    cout<<"-------- [all variables]"<<endl;
    auto varmapall=xfg();
    for(auto&&[name,value]:xfg())cout<<name<<": "<<value<<endl;

    // print a named variable
    cout<<"-------- [named variable (returns std::optional<string>)]"<<endl;
    string varname="junk";
    auto varopt=xfg(varname);
    cout<<"junk: "<<(varopt?varopt.value():"<not set>"s)<<endl;

    // print all a list of named variables
    cout<<"-------- [named variables (returns map<string,optional<string>>)]"<<endl;
    auto varmap=xfg({"system.user","system.pwd","system.userathost","junk"});
    for(auto&&[name,value]:varmap)cout<<name<<": "<<(value?value.value():"<not-set>")<<endl;

    // print all variables matching a regular expression
    cout<<"-------- [regular expression (returns map<string,string>)]"<<endl;
    std::regex rex{"system\\.[a-zA-Z0-9\\.]+"};
    auto sys=xfg(rex);
    for(auto&&[name,value]:sys)cout<<name<<": "<<value<<endl;

    // print all variables inside the 'system' namespace using a 'BasicExtractor'
    cout<<"-------- [variables in namespace, use BasicExtractor (returns map<string,string>)]"<<endl;
    auto sysns=xfg.basicx().ns("system");
    for(auto&&[name,value]:sysns)cout<<name<<": "<<value<<endl;

    // get all variables as Value objects
    cout<<"-------- [variables as Value objects - we know the type of the variable (i.e. int or string)]"<<endl;
    auto vmap=xfg.asValue();
    for(auto&&[name,value]:vmap){
      cout<<name<<": ";
      visit([](auto&&v){
        using V=std::decay_t<decltype(v)>;
        if constexpr(std::is_same_v<V,std::string>)cout<<v<<"[string]";
        else cout<<v<<"[int]";
      },value);
      cout<<"]"<<endl;
    }
  }
  catch(exception const&e){
    cerr<<"exception: "<<e.what()<<endl;
    return 1;
  }
}
