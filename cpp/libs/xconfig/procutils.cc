// (C) Copyright Hans Ewetz 2018. All rights reserved.
#include "xconfig/procutils.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
namespace xconfig{

// close a file descriptor
// (returns errno)
int eclose(int fd){
  while(close(fd)<0&&errno==EINTR);
  return errno;
}
// parse child exit status
// (returns std::nullopt if no errors, else an error string)
optional<string>parseexitstat(int stat){
  // check if child terminated normally
  if(WIFEXITED(stat)){
    int exitcode=WEXITSTATUS(stat);
    if(exitcode==0)return nullopt;
    return string("exit status: ")+to_string(exitcode);
  }else
  if(WIFSIGNALED(stat)){
    if(WCOREDUMP(stat)){
      int signal=WTERMSIG(stat);
      string msg=string("child coredumped from signal: ")+to_string(signal);
      return optional(msg);
    }else{
      int signal=WTERMSIG(stat);
      string msg=string("child terminated from signal: ")+to_string(signal);
      return msg;
    }
  }else{
    string msg=string("child terminated for unknown reason");
    return msg;
  }
}
// spawn child process setting up stdout and stdin as a pipe
optional<string>spawnpipchld(string const&file,vector<string>args,int&fdread,int&fdwrite,int&cpid,bool diewhenparentdies){
  // create pipe between child and parent
  int fromChild[2];
  int toChild[2];
  if(pipe(toChild)!=0||pipe(fromChild)!=0)return "failed creating pipe: "s+strerror(errno);

  // fork child process
  int pid=fork();
  if(pid==0){ // child
    // die if parent dies so we won't become a zombie
// NOTE! should be able to select signal ... (?)
    if(diewhenparentdies&&prctl(PR_SET_PDEATHSIG,SIGHUP)<0)return "failed call to prctl(...): "s+strerror(errno);

    // dup stdin/stdout ---> pipe, and close original pipe fds
    eclose(0);eclose(1);                            // NOTE! Must check for errors
    dup(toChild[0]);dup(fromChild[1]);
    eclose(toChild[1]);eclose(fromChild[0]);
    eclose(toChild[0]);eclose(fromChild[1]);

    // setup for calling execv 
    // (we don't care if we leak memory since we'll overlay process using exec)
    char*const*tmpargs=(char*const*)malloc((args.size()+1)*sizeof(char*const*));
    if(tmpargs==0)return "failed call to malloc(...): "s+strerror(errno);

    char**ptmpargs=const_cast<char**>(tmpargs);
    for(std::size_t i=0;i<args.size();++i)ptmpargs[i]=const_cast<char*>(args[i].c_str());
    ptmpargs[args.size()]=0;
    // execute child process
    if(execvp(file.c_str(),tmpargs)<0){
      return "failed executing execl: "s+strerror(errno);
    }else{
      // should never get here
      return "failed executing execl: "s+strerror(errno);
    }
  }else
  if(pid>0){ // parent
    // close fds we don't use
    eclose(fromChild[1]);eclose(toChild[0]);

    // set return parameters and return child pid
    fdread=fromChild[0];
    fdwrite=toChild[1];
    cpid=pid;
    return nullopt;
  }else{
    // fork failed
    return "failed fork: "s+strerror(errno);
  }
  return nullopt;
}
// execute a program and capture output into a string
// (of ret.first == true, output is in res->second, elase error is in ret->second)
pair<bool,string>execprog(string file,vector<string>args){
  int fdread;
  int fdwrite;
  int cpid;
  auto res=spawnpipchld(file,args,fdread,fdwrite,cpid,true);
  if(res)return pair(false,res.value());

  // read data from pipe ...
  eclose(fdwrite);
  stringstream str;
  char c;
  while(read(fdread,&c,1)==1)str<<c;    // NOTE! should handle error here
  eclose(fdread);

  // wait for child and get exit status code
  int stat;
  if(waitpid(cpid,&stat,0)!=cpid)return pair(false,"failed waiting for child process, err: "s+strerror(errno));

  // get exit status
  auto exitstaterr=parseexitstat(stat);
  if(exitstaterr)return pair(false,exitstaterr.value());

  // no errors
  string ret=str.str();
  if(ret.length()&&ret[ret.length()-1]=='\n')ret=ret.substr(0,ret.length()-1);
  return pair(true,ret);
}
}
