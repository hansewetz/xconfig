# xconfig
**xconfig** implements a C++ API together with utility programs for parsing configuration files.

**xconfig** configurations integrates closely with a UNIX type environment. Access to environment variables as well as output from UNIX commands is transparenly reflected in the configuration language. A C++ API together with a command line program are used for extracting data from configuration files.

**xconfig** is useable as it stands right now. Several improvemnets and extensions are planned. For example, a Python API mirroring the C++ API will be implemented. The configuration language will be updated to transparently support sequences of repeated blocks of information.

# Installation
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.
See deployment for notes on how to deploy the project on a live system.

## Prerequisites
What things you need to install the software:
* [gcc](https://gcc.gnu.org/svn.html) supporting C++17 (tested with gcc 9.0 but should work with gcc 8.X)
* [cmake](https://cmake.org/download/) version 3.12.2 or higher
* [libboost_program_options.so](https://www.boost.org/doc/libs/1_58_0/more/getting_started/unix-variants.html) 1.57 or higher


<details>
  <summary>Installing gcc 9.x</summary>
  
If you don't have gcc 8 or 9 installed, you can pull the latest of trunc and compile it:
```bash
$>export INSTALLHOME=<your-installation-path>
$>
$># ------- install isl
$># download isl-isl-0.19
$>mkdir build && cd build
$>.././configure --prefix=${INSTALLHOME}
$>make && make check && make install
$>
$># ------- install gmp
$># download gmp-6.1
$>mkdir build && cd build
$>.././configure --prefix=${INSTALLHOME}
$>make && make check && make install
$>
$># ------- install mpfr
$># download mpfr-4.0.1
$>mkdir build && cd build
$>.././configure --prefix=${INSTALLHOME} --with-gmp=${INSTALLHOME}
$>make && make check && make install
$>
$># ------- install mpc
$># download mpc-1.1.0
$>mkdir build && cd build
$>.././configure --prefix=${INSTALLHOME} --with-gmp=${INSTALLHOME}
$>make && make check && make install
$>
$># ------- install gcc latest
$>mkdir gcc-trunk && cd gcc-trunk
$>svn checkout svn://gcc.gnu.org/svn/gcc/trunk .
$>mkdir build && cd build
$>././configure --prefix=${INSTALLHOME}   --with-isl-include=${INSTALLHOME}/include --with-isl-lib=${INSTALLHOME}/lib --disable-multilib --with-gmp=${INSTALLHOME} --with-mpfr=${INSTALLHOME} --with-mpc=${INSTALLHOME}
make && make check && make install
```
</details>

<details>
  <summary>Installing libboost_program_options.so.1.58.0</summary>
  
If you don't have **libboost_program_options** installed, please follow these instructions:
```bash
$># download boost v1.58 or higher
$>export INSTALLHOME=<your-installation-path>
$> ./bootstrap.sh --prefix=${INSTALLHOME} --with-libraries=program_options
$>./b2
$>./b2 install
```
</details>
  
<details>
  <summary>Installing cmake-3.12</summary>
  
If you don't have **cmake** version 3.12 or higher installed, please follow these instructions:
```bash
$># download cmake-3.12.2 or higher
$>export INSTALLHOME=<your-installation-path>
$>mkdir build && cd build
$>cmake -DCMAKE_INSTALL_PREFIX=${INSTALLHOME} ..
$>make && make install
```
</details>


## Installing xconfig
Step by step recipie for compiling and installing **xconfig** (assuming gcc 9.x, cmake 3.12, libboost_program_options.so are installed):
```bash
$>mkdir xconfig && cd xconfig
$>git clone https://github.com/hansewetz/xconfig.git .
$>mkdir build && cd build
$>export CXX=<full path to g++ compiler>            # /usr/bin/g++
$>export CC=<full path to gcc compiler>             # /usr/bin/gcc
$>cmake -DCMAKE_INSTALL_PREFIX=<install-path> ..
$>make && make install
```

The installation directory is populated with:
```bash
.
├── ...
├── <install-path>
│   ├── bin                    # xconfig binaries
│   ├── lib                    # xconfig libraries
│   ├── include                # xconfig C++ header files
│   ├── examples               # sample programs
└── ...
```

# Getting Started with **xconfig**

## Example 1
Type or copy/paste the following into **test1.cfg**:
```bash
namespace system{
  user = $USER                           # user from environment variable
  pwd = `pwd`                            # execute bash command - current work directory
  userathost = @"%{user}@`hostname`"     # string interpolation - concatenate variable with `hostname`
}
```
Print configuration using **xconfig**:
```bash
xconfig test1.cfg
```
The output is:
```bash
system_pwd="/home/user/hansewetz"
system_user="hansewetz"
system_userathost="hansewetz@dumbo"
```
A short explanation:
  * environment variables are accessed using 'dollar' notation
  * bash commands can be executed using 'backquote' notation
  * configuration variables are access using 'percentage' notation or directly with only the name
  * environment variables, configuration variables and bash commands can be evaluated using string interpolation by prefixing the string with an 'at' sign (@)
  * names can be scoped using namespaces

## Example 2
The following example shows how to use the **xconfig** C++ API:
```C++
#include "xconfig/XConfig.h"
#include <iostream>
using namespace std;
using namespace xconfig;

int main(){
  // read and evaluate configuration file and print variables
  XConfig xfg("test1.cfg");
  for(auto&&[name,value]:xfg())cout<<name<<": "<<value<<endl;
}
```

To compile/link the program:
```bash
g++ -o test1 test1.cc -m64 -fPIC -Wall -pedantic -Werror -std=c++2a -Wno-deprecated -Wno-register -D_GLIBCXX_USE_CXX11_ABI=0 -I<xconfig-install-path>/include -L<xconfig-install-path>/lib -lxconfigl
```

When running the program we get this output:
```bash
system.pwd: /home/user/hansewetz
system.user: hansewetz
system.userathost: hansewetz@dumbo
```

# Versioning
Not yet done

# Deployment
Not yet done

# In depth: language, API and design
Not yet done

## Language
Not yet done

## API
Not yet done

## Design
Not yet done

# Development
The following is planned:
* add versioning to project (generate header file within the cmake build that contains version number)
* implement Python based API
* extend language to support repeated groups of structurally similar blocks of definitions
* better/more documentation
* possibly - port to lower version of gcc (i.e. C++11)

# License
This project is licensed under the MIT License - see the [LICENSE.md](https://choosealicense.com/licenses/mit/) file for details

# Authors, emails, blogs etc.
* author: Hans Ewetz
* email: hansewetz@hotmail.com
* blog: https://hansewetz.blogspot.com


