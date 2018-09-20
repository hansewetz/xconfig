#pragma once
#include <string>
#include <vector>
#include <set>
#include <optional>
#include <iosfwd>
namespace xconfig{
// symbol table used during parsing/compilatin
class Symtab{
public:
  // debug print method
  friend std::ostream&operator<<(std::ostream&os,Symtab const&st);

  // constants
  constexpr static char NSSEP='.';

  // ctor, assign, dtor
  Symtab()=default;

  // ns management
  void pushns(std::string const&name);
  void popns();
  std::string currentns()const noexcept;

  // symbol management
  std::optional<std::string>lookupsym(std::string const&name)const;
  std::string addsym(std::string const&name);
  bool isSimpleSymbol(std::string const&name)const;
  std::string fullyQualifiedName(std::string const&name)const;
private:
  // private data
  std::vector<std::string>nsstack_; // track current namespace
  std::set<std::string>nstab_;      // all namespaces
  std::set<std::string>symtab_;     // symbol table containing all fully qualified symbols
};
}
