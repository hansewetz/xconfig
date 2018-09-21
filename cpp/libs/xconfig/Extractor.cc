// (C) Copyright Hans Ewetz 2018. All rights reserved.
#include "xconfig/Extractor.h"
using namespace std;
namespace xconfig{

// ctor
Extractor::Extractor(shared_ptr<Mmvm>vm):vm_(vm){
}
// get vm
shared_ptr<Mmvm>Extractor::vm()const{return vm_;}
}
