# xconfig
**xconfig** implements a C++ API together with utility programs for parsing configuration files.

**xconfig** configurations integrates closely with a UNIX type environment. Access to environment variables as well as access to output from UNIX commands is transparenly reflected in the configuration language. A C++ API together with a command line program are used for extracting data from configuration files.

Eventhough **xconfig** is a first cut implementation of an idea that needs many improvements it is useable as it stands right now. Several updates and extensions are planned. For example, a Python API mirroring the C++ API will be implemented. The configuration language will also be updated to transparently support sequences of repeated blocks of information.

# A small example

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
  * configuration variables are accessed using 'percentage' notation or directly by the name
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

To compile/link the program do:
```bash
g++ -o test1 test1.cc -m64 -fPIC -Wall -pedantic -Werror -std=c++2a -Wno-deprecated -Wno-register -D_GLIBCXX_USE_CXX11_ABI=0 -I<xconfig-install-path>/include -L<xconfig-install-path>/lib -lxconfigl
```

When running the program we get this output:
```bash
system.pwd: /home/user/hansewetz
system.user: hansewetz
system.userathost: hansewetz@dumbo
```
# Installation
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

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

# Versioning
<p>
The version of the <b>xconfig</b> API  is located in the <code>xconfig/version.h</code> header file. The version is specified through two <code>int</code> C++ variables:
</p>

<ul>
  <li><code>constexpr const static int XCONFIG_VERSION_MAJOR</code></li>
  <li><code>constexpr const static int XCONFIG_VERSION_MINOR</code></li>
</ul>
<p>
You can print the version of the current installation by executing: <code>xconfig --version</code>.
</p>

# In depth: language, API and design
Not yet done

## Language
<p>
The language follows a C like style where variable names consists of alphanumeric characters including the underscore character.
Variables can be assigned string or integer values:

<pre class="brush: bash">
user = "hansewetz"             # 'user' is a string
maxthread = 20                 # 'maxthreads' is an integer
</pre>
</p>

<p>
Environment variables are accessed using a <b>bash</b> style notation:
<pre class="brush: bash">
user = ${USER}             # 'user' is assigned the value of environment variable 'USER'
user1 = $USER              # works without braces also
</pre>
</p>

<p>
Normal variables are accessed by dircetly using their name or by prefixing them with '%':
<pre class="brush: bash">
user = @"${USER}"                        # 'user' is assigned the value of environment variable 'USER'
userAtMachine= @"%{user}@`hostname -i`"  # access variable 'USER' inside a string
</pre>
</p>

<p>
Commands can be executed by enclosing the the command inside back-quotes:
<pre class="brush: bash">
machine = `hostname`   # get name of machine
</pre>
</p>


<p>
The <i>plus</i> operator concatenate strings and adds integers:
<pre class="brush: bash">
user = "hans"+"ewetz"         # user = "hansewetz"
minthreads = 10
maxthreads = minthreads + 8   # maxthreads = 18
</pre>
</p>

<p>
An expression on the right hand side of the assignment operator can access a variable using <i>dot</i> separated namespaces:
<pre class="brush: bash">
namespace system{
  user = $USER                     # user = "hansewetz"
  machine = `hostname -i`          # machine = "10.0.0.4"
}
userAtMachine = @"%{system.user}@%{system.machine}"   # userAtMachine = "hansewetz@10.0.0.4"
</pre>
</p>

<p>
As the previous example showed strings can be interpolated.
During interpolation environment variable, normal variables as well as commands are interpolated.
A variation of the previous example is shown here:
<pre class="brush: bash">
namespace system{
  user = @"$USER"                  # hansewetz
  machine = `hostname -i`          # machine = "10.0.0.4"
}
userAtMachine = @"%{system.user}@%{system.machine}"   # userAtMachine = "hansewetz@10.0.0.4"
</pre>
</p>

<p>
There are some restrictions to how variables can be used.
A variable cannot be assigned to once it has a value.
Neither can a namespace qualified variable be used on the left hand side of the assignment operator.
</p>

## API
Not yet done

## Design
<p>
The design is relatively straight forward:
<ul>
  <li><b>bison</b>/<b>flex</b> is used for parsing the configuration</li>
  <li>during parsing write opcodes for a <i>virtual machine</i></li>
  <li>execute the opcodes with the virtual machine</li>
  <li>use an <i>extractor</i> to retrieve data from the memory of the virtual machine</li>
</ul>
</p>

<p>
The design is shown in this diagram:
</p>

<img src="data:image/png;base64,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"/>

<p>
The <i>virtual machine</i> is implemented as a simple stack machine tailored specifically for this project.
The name of the virtual machine is MMVM - <i>Mickey Mouse Virtual Machine</i>.
The MMVM currently supports 13 opcodes.
Among them are simple operation such as 'push value on stack' or 'store value in memory'.
More complex operations such as 'evaluate a command in a shell and store output on stack' are also supported.
</p>

<p>
One might wonder if it is not overkill to implement a VM just for parsing a configuration file.
As it turns out, having a VM available makes it almost effortless to support and experiment with new features in the configuration language.
Since the MMVM implementation is only around 330 lines of C++ code it can be implemented in only a few hours.
</p>

<p>
The MMVM currently supports <b>string</b>s and <b>int</b>egers.
The support is not extensive. Two integers can be added, two strings can be concatenated and integers can be added.
</p>

<p>
Evaluation of a configuration file is done in two steps:
<ul>
  <li>compile the file into a program(generate a program consisting of op codes for MMVM)</li>
  <li>execute program (sequence of opcodes</li>
</ul>
</p>

<p>
Namespaces are used to scope variables.
The full name of a variable is a series of <i>dot</i> separated namespaces followed by the name of the variable.
It is not necessary to use the full name of a variable.
When namespaces are left out, the compiler looks for the variable in the enclosing scope(s) .
For example:
</p>

<p>
<pre class="brush: bash">
# test1.cfg
a = 18
namespace ns{
  namespace ns1{
    b = 19
    namespace ns2{
      c = b       # 'b' is found one step up
      d = ns1.b   # same as previous
      e = a       # 'a' is found in outer most scope
    }
  }
}
</pre>
</p>

<p>
The output from executing: <b>xconfig test1.cfg</b>:
</p>

<p>
<pre class="brush: bash">
a="18"
ns_ns1_b="19"
ns_ns1_ns2_c="19"
ns_ns1_ns2_d="19"
ns_ns1_ns2_e="18"
</pre>
</p>


<p>
When interpolating strings I would like to be able to handle namespaces the same way as they are handled when referencing variables outside string interpolation.
In order to reference variables without specifying the full namespace path, we need a table that contains information about the location of a variables within namespaces.
This is done by building the table at compilation time and using it at runtime during string interpolation.
</p>

# Development
The following is planned:
* add git commit SHA to 'version.h' file (via cmake)
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


