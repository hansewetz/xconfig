#pragma once
#include "xconfig/BasicExtractor.h"
#include "xconfig/Mmvm.h"
#include <optional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#include <regex>
#include <iosfwd>
namespace xconfig{
// forward decl
class Mmvm;

// interface to xconfig system
class XConfig{
public:
  // ctor, assign, dtor
  XConfig();
  XConfig(std::string const&cfgpath);
  XConfig(std::istream&is,std::string const&name);
  XConfig(XConfig const&)=delete;
  XConfig(XConfig&&)=delete;
  XConfig const&operator=(XConfig const&)=delete;
  XConfig const&operator=(XConfig&&)=delete;
  ~XConfig()=default;

  // data extractors
  BasicExtractor const&basicx()const;

  // simplified extraction operations directly on XConfig objects
  // (delegates to a BasicExtractor object)
  std::map<std::string,std::string>operator()()const;
  std::optional<std::string>operator()(std::string const&name)const;
  std::map<std::string,std::optional<std::string>>operator()(std::vector<std::string>const&v)const;
  std::map<std::string,std::string>operator()(std::regex const&r)const;

  // get by Mmvm::Value
  std::map<std::string,Mmvm::Value>asValue()const;
  std::optional<Mmvm::Value>asValue(std::string const&name)const;
  // ... NOTE! add more methods for retrieving by Mmvm::Value ...

  // basic methods for dumping information from vm
  void dumpprog(std::ostream&os)const;
  void dumpstack(std::ostream&os)const;
  void dumpmem(std::ostream&os)const;
  void dumpsymtab(std::ostream&os)const;

  // NOTE! Not yet done

private:
  // compile and run from an input stream
  void compileAndRun(std::istream&is,std::string const&name);

  // attributes
  std::shared_ptr<xconfig::Mmvm>vm_;
  BasicExtractor basicx_;
};
}
