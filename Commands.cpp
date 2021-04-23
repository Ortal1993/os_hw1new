#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <limits.h>

using namespace std;

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

const std::string WHITESPACE = " \n\r\t\f\v";


string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

//SmallShell::SmallShell() {
// TODO: add your implementation
//}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	// For example:
/*
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if ...
  .....
  else {
    return new ExternalCommand(cmd_line);
  }
  */

  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(WHITESPACE));///originally \n

  if(firstWord.compare("chprompt") == 0){
      return new RedirectionCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
      return new ShowPidCommand(cmd_line);
  }
  else if (firstWord.compare("pwd") == 0) {
      return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("cd") == 0) {
      return new ChangeDirCommand(cmd_line);
  }
  /*else if (firstWord.compare("jobs") == 0){
      return new JobsCommand(cmd_line, &this->jobsList);
  }
  else if (firstWord.compare("kill") == 0){
      return new KillCommand(cmd_line, &this->jobsList);
  }
  else if (firstWord.compare("fg") == 0){
      return new ForegroundCommand(cmd_line, &this->jobsList);
  }
  else if (firstWord.compare("bg") == 0){
      return new BackgroundCommand(cmd_line, &this->jobsList);
  }
  else if (firstWord.compare("quit") == 0){
      return new QuitCommand(cmd_line, &this->jobsList);
  }*/
  return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
    Command* cmd = CreateCommand(cmd_line);
    cmd->execute();
    ///delete cmd;
}

std::string SmallShell::GetPrompt() {
    return this->prompt;
}

Command::Command(const char *cmd_line): cmd_line(cmd_line), sms(SmallShell::getInstance()){
    //this->sms = SmallShell::getInstance();

    string toParse = (string)this->cmd_line;
    toParse = _trim(toParse);
    while(toParse.length() > 0){///arguments[0] = command
        int argLength = toParse.find_first_of(WHITESPACE);
        if(argLength == -1){
            string currArgument = toParse.substr(0, argLength);
            this->arguments.push_back(currArgument);
            break;
        }
        string currArgument = toParse.substr(0, argLength);
        this->arguments.push_back(currArgument);
        toParse = toParse.substr(argLength + 1);
        toParse = _ltrim(toParse);
    }
}

string Command::GetArgument(int argNum) {
    /*if (argNum < 1 || argNum > this->GetNumOfArgs()){///arguments[0] = command
        return "";
    }*/
    return this->arguments[argNum];
}

int Command::GetNumOfArgs() {
    return this->arguments.size();
}

///func 1 - showpid
void ShowPidCommand::execute() {
    std::cout << "smash pid is: " << getSmallShell().getPid() << endl;
}

///func 2 - chprompt
void RedirectionCommand::execute(){
    int numOfArgs = this->GetNumOfArgs();
    SmallShell& sm = getSmallShell();
    if (numOfArgs == 1){///arguments[0] = command
       sm.setPrompt("smash");
    }else{///numOfArgs > 1
        sm.setPrompt(this->GetArgument(1));
    }
}

///func 3 - pwd
void GetCurrDirCommand::execute() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working dir: " << cwd << endl;
    } else {
        std::cout << "error: "  << endl;
    }
}

///func 4 - cd
//ChangeDirCommand::ChangeDirCommand(const char *cmd_line) :
                //BuiltInCommand(cmd_line){
//}

void ChangeDirCommand::execute() {
    int numOfArgs = this->GetNumOfArgs();
    SmallShell& sm = getSmallShell();
    string lastPwd = sm.getLastPwd();
    string currentPwd = sm.getCurrentPwd();
    if (numOfArgs > 2){
        //print error
        std::cerr << "smash error: cd: too many arguments" << endl;
    }else if (numOfArgs == 2){
        string arg = this->GetArgument(1);
        if (arg == "-" && lastPwd != ""){
            const char * path = lastPwd.c_str();
            int error = chdir(path);
            if(error == -1){//syscall failed
                //print error, errno
                std::cout << "error: syscall failed. lastPWD: " << lastPwd << ", path: " << path << endl;
            }
            else{
                sm.setLastPwd(currentPwd);
                sm.setCurrentPwd(lastPwd);
            }
        } else if (arg == "-" && lastPwd == ""){
            //print error no OLDPWD
            std::cout << "smash error: cd: OLDPWD not set"  << endl;
        } else {
            const char * path = arg.c_str();
            int error = chdir(path);
            if(error == -1){//syscall failedl
                //print error, errno
                std::cout << "error: sysCallFail2"  << endl;
            }
            else{
                if (currentPwd == ""){//if *currentPwd is null, then it is the first time doing cd
                    sm.setLastPwd("");
                }
                else{
                    sm.setLastPwd(currentPwd);
                }
                sm.setCurrentPwd(const_cast<char *>(path));
            }
        }
    }
}

///func 5 - jobs

///func 6 - kill
/*void KillCommand::execute() {
    int numOfArgs = this->GetNumOfArgs();
    if (numOfArgs != 3) {///arguments[0] = command ///if we kill -  9 7 - is it legal?
        cerr << "smash error: kill: invalid arguments" << endl;
    }
    string numStr = GetArgument(1);
    int i = 0;
    while(numStr.at(i) == '-'){
        i++;
    }
    if(i == 1){
        numStr = numStr.substr(1);
        int num = stoi(numStr);
        if(num < 1 || num > 64){
            //print error;
        }else{
            string jobStr = GetArgument(2);
            int jobId = stoi(jobStr); ///std::invalid_argument
            if(ptrJobs->GetJobsMap()->find(jobId) != ptrJobs->GetJobsMap()->end()){
                cout << "signal number " << num << "was sent to pid" << ptrJobs->GetJobsMap()->find(jobId)->second.GetProcessID();
                ptrJobs->GetJobsMap()->find(jobId)->second.SetSignal(num);///we need it?
            }else{
                cerr << "smash error: kill: job-id " << jobId << "does not exist" << endl;
            }
        }
    }else{
        //print error?
    }
}*/

///func 7 - fg

///func 8 - bg

///func 9 - quit
/*void QuitCommand::execute() {

}*/

/*const char* SmashExceptions::what() const noexcept{
    return what_message.c_str();
}

KillInvalidArg::KillInvalidArg() : SmashExceptions("smash error: kill: invalid arguments"){}*/