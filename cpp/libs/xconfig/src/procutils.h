#pragma once
#include <vector>
#include <optional>
namespace xconfig{

// close an fd with error checking
// (returns errno)
int eclose(int fd);

// parse child exit status
// (returns std::nullopt if no errors, else an error string)
std::optional<std::string>parseexitstat(int stat);

// spawn child process setting up stdout and stdin as a pipe
std::optional<std::string>spawnpipchld(std::string const&file,std::vector<std::string>args,int&fdread,int&fdwrite,int&cpid,bool diewhenparentdies);

// execute a program and get output into a string
std::pair<bool,std::string>execprog(std::string file,std::vector<std::string>args);
}
