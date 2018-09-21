// (C) Copyright Hans Ewetz 2018. All rights reserved.
#include "xconfig/version.h"
#include "xconfig/XConfig.h"
#include "xconfig/stringutils.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <strstream>
#include <optional>
#include <map>
using namespace std;
using namespace xconfig;
namespace po=boost::program_options;

namespace{
// variables set in cmdline parsing
bool program_dump=false;
bool memory_dump=false;
bool export_var=false;
bool single_quote=false;
bool noquote=false;
char ns_separator='_';
string regex_filter="";
vector<string>variable_filter;
vector<string>namespace_filter;
optional<string>inputfile;

// cmdline optins
po::options_description visible_options{string("usage: [-h|-P|-D] [<inputfile>]")};

void usage(){
  std::cerr<<visible_options;
  std::exit(1);
}
void printversion(){
  cout<<XCONFIG_VERSION_MAJOR<<"."<<XCONFIG_VERSION_MINOR<<endl;
  std::exit(0);
}
// process cmd line params
void cmdline(int argc,char*argv[]){
  // positional parameters
  po::positional_options_description positional_options;
  positional_options.add("inputfile", -1);

  // hidden options (will not show up in '--help')
  po::options_description hidden_options("hidden options");
  hidden_options.add_options()("inputfile",po::value<string>(),"");

  // setup visible options (will show up in '--help')
  visible_options.add_options()("help,h","help");
  visible_options.add_options()("version,v","print version number of xconfig and exit");
  visible_options.add_options()("program-dump,P","dump compiled code (for debug purpose)");
  visible_options.add_options()("memory-dump,M","dump memory (all variables) after compiling and running configuration (for debug purpose)");
  visible_options.add_options()("single-quote,S","enclose value in single quotes ('abc') instead of in couble quaotes (\"abc\")");
  visible_options.add_options()("noquote,N","do not encluse value in quote");
  visible_options.add_options()("export,e","add 'export' before each variable name");
  visible_options.add_options()("separator,s",po::value<string>(),"character to be used as namespace separator - default '_'");
  visible_options.add_options()("regex-filter,r",po::value<string>(),"regular expression used to filter variables - filter all variables");
  visible_options.add_options()("variables,V",po::value<string>(),"list of space separated variable names (within a single/double quoted string) to include in output");
  visible_options.add_options()("namespaces,n",po::value<string>(),"list of space separated namespaces (within a single/double quoted string) to include in output");

  // concatenate all options
  po::options_description all_options;
  all_options.add(visible_options).add(hidden_options);

  // process options
  po::variables_map vm;
  po::store(po::command_line_parser(argc,argv).options(all_options).positional(positional_options).allow_unregistered().run(),vm);
  po::notify(vm);

  // get command line parameters
  if(vm.count("help"))usage();
  if(vm.count("version"))printversion();
  if(vm.count("program-dump"))program_dump=true;
  if(vm.count("memory-dump"))memory_dump=true;
  if(vm.count("single-quote"))single_quote=true;
  if(vm.count("noquote"))noquote=true;
  if(vm.count("export"))export_var=true;
  if(vm.count("separator")){
    // make sure separator has a valid value
    // (must only contain characters in: [a-zA-Z0-9_])
    string ns_separator_str=vm["separator"].as<string>();
    if(ns_separator_str.length()!=1)throw runtime_error("namespace separator must be one character long");
    std::regex re("[a-zA-Z0-9_]*");
    if(!regex_match(ns_separator_str,re))throw runtime_error("invalid character(s) in namespace separator - separator must match [a-zA-Z0-9_]*");
    ns_separator=ns_separator_str[0];
  }
  if(vm.count("regex-filter"))regex_filter=vm["regex-filter"].as<string>();
  if(vm.count("variables"))variable_filter=splitonblanks(vm["variables"].as<string>());
  if(vm.count("namespaces"))namespace_filter=splitonblanks(vm["namespaces"].as<string>());
  if(vm.count("inputfile"))inputfile=vm["inputfile"].as<string>();

  // if no variables have been specified and no regex has been specified and no namspaces have been specified then include all variables
  if(vm.count("regex-filter")==0&&vm.count("variables")==0&&vm.count("namespaces")==0)regex_filter="[a-zA-Z0-9_\\.]+";
}
// write one variable to stdout
void writevar(ostream&os,string const&name,string const&value){
  string tmp_name=name;
  for(auto&c:tmp_name)if(c=='.')c=ns_separator;
  if(export_var)cout<<"export ";
  if(noquote){
    os<<tmp_name<<"="<<value<<endl;
  }else{
    char quote=single_quote?'\'':'"';
    os<<tmp_name<<"="<<quote<<value<<quote<<endl;
  }
}
}
// 'xconfig' main program
int main(int argc,char*argv[]){
  try{
    // get cmdline params
    cmdline(argc,argv);

    // compile and run configuration file
    // (if no input file is specified we read from stdin)
    unique_ptr<XConfig>xfg;
    if(inputfile)xfg.reset(new XConfig(inputfile.value()));
    else xfg.reset(new XConfig());

    // process vm memory after compiling and running configuration file
    if(program_dump){
      cout<<"<program-dump>"<<endl;
      xfg->dumpprog(cout);
    }
    if(memory_dump){
      cout<<"<memory-dump>"<<endl;
      xfg->dumpmem(cout);
    }
    // collect variables from regex to include
    map<string,string>varmap;
    auto&&varmap_regex{(*xfg)(regex(regex_filter))};
    for(auto&&[name,value]:varmap_regex)varmap[name]=value;

    // collect variables specified by name to include
    auto&&varmap_var{(*xfg)(variable_filter)};
    for(auto&&[name,value]:varmap_var){
      if(value)varmap[name]=value.value();
    }
    // collect variables from namespaces to include
    auto&&varmap_ns{xfg->basicx().ns(namespace_filter)};
    for(auto&&[name,value]:varmap_ns)varmap[name]=value;

    // write variables specified by name
    for(auto&&[name,value]:varmap)writevar(cout,name,value);
    
    // NOTE! Not yet done
    // - add option for generating C++ skeleton code for parsing file
    // - add option for selecting based on namespaces (internally would use regular expression) (BasicExtractor::ns(...)
    // - add 'negative' option with regex for what not to include
    // - add 'negative' option with names for what not to include
  }
  catch(exception const&e){
    cerr<<"error: "<<e.what()<<endl;
    return 1;
  }
}
