#pragma once
#include <memory>
namespace xconfig{

// forward decl
class Mmvm;

// basic extrcator
class Extractor{
public:
  // ctor,assign,dtor
  Extractor(std::shared_ptr<Mmvm>vm);
  Extractor(Extractor const&)=default;
  Extractor(Extractor&&)=default;
  Extractor&operator=(Extractor const&)=default;
  Extractor&operator=(Extractor&&)=default;
  virtual~Extractor()=default;

  // get vm
  std::shared_ptr<Mmvm>vm()const;
private:
  std::shared_ptr<Mmvm>vm_;
};
}
