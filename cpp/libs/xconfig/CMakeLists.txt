# add include path to where generated flex and bison headers are
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# create version.h file
configure_file(
  "version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
  )

# setup for flex/bison code generation
find_package(FLEX)
flex_target(scanner scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)
find_package(BISON)
bison_target(parser parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)

# build library
add_library (xconfigl SHARED 
  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  BasicExtractor.cc
  driver.cc
  Extractor.cc
  Mmvm.cc
  MmvmError.cc
  procutils.cc
  stringutils.cc
  Symtab.cc
  XConfig.cc)

# install library
install(TARGETS xconfigl DESTINATION lib)

# install header files
install (FILES 
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
  "BasicExtractor.h"
  "driver.h"
  "Extractor.h"
  "MmvmError.h"
  "Mmvm.h"
  "procutils.h"
  "scanner.h"
  "stringutils.h"
  "Symtab.h"
  "XConfig.h"
  DESTINATION include/xconfig)
