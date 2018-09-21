// (C) Copyright Hans Ewetz 2018. All rights reserved.
#include "xconfig/XConfig.h"
#include <iostream>
#include <strstream>
using namespace std;
using namespace xconfig;

int main(){
  try{
    XConfig xfg("example1.cfg");

    // dump some info
    //cout<<"-------- program"<<endl;
    //xfg.dumpprog(cout);
    //cout<<"-------- symtab"<<endl;
    //xfg.dumpmem(cout);

    // get all variables
    cout<<"-------- cfg all data"<<endl;
    auto varmapall=xfg();
    for(auto&&[name,value]:xfg())cout<<name<<": "<<value<<endl;

    // get and print variables
    cout<<"-------- [variables] cfg subset of data"<<endl;
    auto varmap=xfg({"system.user","system.pwd","system.userathost"});
    for(auto&&[name,value]:varmap)cout<<name<<": "<<(value?value.value():"<not-set>")<<endl;

    // print all variables inside the 'sys' namespace
    cout<<"-------- [regular expression] cfg data [system] namespace"<<endl;
    std::regex rex{"system\\.[a-zA-Z0-9\\.]+"};
    auto sys=xfg(rex);
    for(auto&&[name,value]:sys)cout<<name<<": "<<value<<endl;

    // print all variables inside the 'system' namespace
    cout<<"-------- [namespace] cfg data [system] namespace"<<endl;
    auto sysns=xfg.basicx().ns("system");
    for(auto&&[name,value]:sysns)cout<<name<<": "<<value<<endl;
  }
  catch(exception const&e){
    cerr<<"exception: "<<e.what()<<endl;
    return 1;
  }
}
