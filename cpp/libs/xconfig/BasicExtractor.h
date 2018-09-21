// (C) Copyright Hans Ewetz 2018. All rights reserved.
#pragma once
#include "xconfig/Extractor.h"
#include "xconfig/Mmvm.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <regex>
namespace xconfig{

// forward decl
class Mmvm;

// basic extrcator
class BasicExtractor:public Extractor{
public:
  // ctor,assign,dtor
  BasicExtractor(std::shared_ptr<Mmvm>vm);
  BasicExtractor(BasicExtractor const&)=default;
  BasicExtractor(BasicExtractor&&)=default;
  BasicExtractor&operator=(BasicExtractor const&)=default;
  BasicExtractor&operator=(BasicExtractor&&)=default;
  virtual~BasicExtractor()=default;

  // get all variables/values as string
  std::map<std::string,std::string>operator()()const;

  // get value as a string for a single variable name
  std::optional<std::string>operator()(std::string const&name)const;

  // get values for a list of names)
  std::map<std::string,std::optional<std::string>>operator()(std::vector<std::string>const&v)const;

  // get values for variables having names matching a regular expression
  std::map<std::string,std::string>operator()(std::regex const&r)const;
  std::map<std::string,std::string>regex(std::string const&rstr)const;

  // get variables/values for a specific namespace
  std::map<std::string,std::string>ns(std::string const&ns)const;
  std::map<std::string,std::string>ns(std::vector<std::string>const&nss)const;

  // NOTE! testing
  std::map<std::string,Mmvm::Value>asValue()const;
  std::optional<Mmvm::Value>asValue(std::string const&name)const;
};
}
