#  c++ pre-processor and compilation flags
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-m64 -fPIC -Wall -pedantic -Werror -std=c++2a -Wno-deprecated -Wno-register)

# setup include path to lib directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory (libs )
add_subdirectory (apps)
add_subdirectory (examples)
