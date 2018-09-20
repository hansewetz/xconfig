#pragma once
#include <string>
#include <set>
#include <vector>
#include <functional>
namespace xconfig{

// forward decl
class Symtab;

// de-escape double quotatinos in a string
std::pair<bool,std::string>deescape(std::string const&str,std::set<char>const&escchars);

// interpolate a string
std::pair<bool,std::string>interpolate(std::string const&str,
                                       std::function<std::pair<bool,std::string>(std::string const&)>const&fenv,
                                       std::function<std::pair<bool,std::string>(std::string const&)>const&fvar,
                                       std::function<std::pair<bool,std::string>(std::string const&)>const&fcmd,
                                       Symtab const&symtab);
// split string on blanks and return a vector
std::vector<std::string>splitonblanks(std::string const&str);
}
