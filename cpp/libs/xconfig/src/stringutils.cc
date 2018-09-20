#include "xconfig/stringutils.h"
#include "xconfig/Symtab.h"
#include <string>
#include <set>
#include <sstream>
#include <iterator>
#include <iostream>
using namespace std;
namespace xconfig{

// helpers
namespace{
// check if a character can be part of an identifier
bool valididc(bool allowdot,char c){
  if(!allowdot)return (c>='a'&&c<'z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_';
  return (c>='a'&&c<'z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='.';
}
}
// de-escape double quotations and escape-chars in a string
pair<bool,string>deescape(string const&str,set<char>const&escchars){
  set<char>echars=escchars;
  echars.insert('\\');
  stringstream ret;
  int ind=0;
  int n=str.size();
  while(ind<n){
    char c=str[ind++];
    if(c=='\\'){
      if(ind==n)return pair(false,"escape character '\\' found at end of string: ");
      c=str[ind++];
      if(!echars.count(c))ret<<'\\';   // we want to preserve other escaped characters since they can be stripped at runtime during interpolation
      ret<<c;
    }else{
      ret<<c;
    }
  }
  return pair(true,ret.str());
}
// interpolate a string
// (escape chars: ['"$%{}\])
// (envvar - $xxx or ${xxx})
// (memvar - %xxx or %{xxx})
// (returns: (true,result) if no errors, (false,errstr) if error)
pair<bool,string>interpolate(string const&str,
                             function<pair<bool,string>(string const&)>const&fenv,
                             function<pair<bool,string>(string const&)>const&fvar,
                             function<pair<bool,string>(string const&)>const&fcmd,
                             Symtab const&symtab){
  static set<char>escchars={'$','%','`'};
  stringstream ret;
  int ind=0;
  int n=str.size();
  while(ind<n){
    char c=str[ind++];

// NOTE! Should look closer at how we handle escape char

    // check if we have an escaped char
    if(c=='\\'){
      if(ind==n)return pair(false,"escape character '\\' found at end of string: ");
      c=str[ind++];
      if(!escchars.count(c))return pair(false,"invalid escape sequence '\\"s+c+"' - can only escape characters: [\\\"$%]");
      ret<<c;
      continue;
    }
    // check if we have a non-escaped character with no special meaning
    if(escchars.count(c)==0){
      ret<<c;
      continue;
    }
    // check if we have an embedded command
    if(c=='`'){
      if(ind==n)return pair(false,"found ` at end of string");
      stringstream scmd;
      c=str[ind++];
      while(c!='`'){
        scmd<<c;
        if(ind==n)return pair(false,"no matching '`' for command");
        c=str[ind++];
      }
      // check if we got a cmd and get it as a string
      if(c!='`')return pair(false,"no matching '`' for command");
      string cmd=scmd.str();

      // execute cmd
      auto cmdres=fcmd(cmd);
      if(!cmdres.first)return pair(false,"failed executing cmd: "s+cmd);
      ret<<cmdres.second;
      continue;
    }
    // we either have an env variable or a normal program variable
    // remember type of variable
    bool isenv=c=='$'?true:false;

    // we have an environment or memory variable (either $xxx, ${xxx}, %xxx or %xxx)
    if(ind==n)return pair(false,"found '"s+c+"' at end of string");
    stringstream sname;
    c=str[ind++];
    if(c=='{'){
      // search for a matching '}' - we can only have chars in [a-zA-Z0-9_]
      bool allowdot=false;
      while(ind<n&&(c=str[ind++])!='}'){
        if(!valididc(allowdot,c))return pair(false,"invalid character: "s+c+" inside interpolated variable name");
        allowdot=!isenv;
        sname<<c;
      }
      // check if we got a name
      if(c!='}')return pair(false,"no matching '}' for '{'");
    }else{
      // search for a character that is not a valid ident char
      bool allowdot=false;
      for(valididc(allowdot,c);;){
        allowdot=!isenv;
        sname<<c;
       if(ind==n)break;
       c=str[ind];
       if(!valididc(allowdot,c))break;
       ++ind;
      }
    }
    // we now have a name for variable (env or var)
    string name=sname.str();
    if(name.length()==0)return pair(false,"found empty name (environment or variable)");

    // depending on if we have an env var or memory variable do something
    if(isenv){
      auto envres=fenv(name);
      if(!envres.first)return pair(false,"failed getting environment variable for name: "s+name);
      ret<<envres.second;
    }else{
      // lookup fully qualified name in symtab
      auto fqname=symtab.lookupsym(name);
      if(!fqname)return pair(false,"symbol: '"s+name+"' not found in current namespace: '"+symtab.currentns()+"' during interpolation");

      // get variable from memory
      auto varres=fvar(fqname.value());
      if(!varres.first)return pair(false,"failed getting variable for name: "s+name);
      ret<<varres.second;
    }
  }
  return pair(true,ret.str());
}
// split string on blanks and return a vector
vector<string>splitonblanks(string const&str){
  istringstream iss(str);
  return vector<string>(istream_iterator<string>(iss),istream_iterator<string>());
}
}
