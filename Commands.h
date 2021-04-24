#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <unistd.h>
#include <map>
#include <ctime>
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

enum STATUS{///???
    BACKGROUND = 0,
    STOPPED = 1,
    /*FINISHED = 2*/
};

class SmallShell;

class Command{
    private:
        const char* cmd_line;///maybe const char*?
        std::vector<std::string> arguments;
        SmallShell& sms;
    public:
        Command(const char* cmd_line);
        virtual ~Command() {};
        virtual void execute() = 0;
        //virtual void prepare();
        //virtual void cleanup();
        // TODO: Add your extra methods if needed
        std::string GetArgument(int argNum);
        int GetNumOfArgs();
        SmallShell& getSmallShell() {return this->sms;}
};

class BuiltInCommand : public Command {
    public:
        BuiltInCommand(const char* cmd_line): Command(cmd_line){}
        virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
    public:
        ExternalCommand(const char* cmd_line);
        virtual ~ExternalCommand() {}
        void execute() override;
};

///func 1 - chprompt
class RedirectionCommand : public Command {///why inherits from Command?
    //private:
        //std::string* ptrPrompt;
    public:
        //explicit RedirectionCommand(const char* cmd_line, std::string* ptrPrompt): Command(cmd_line), ptrPrompt(ptrPrompt){};
        explicit RedirectionCommand(const char* cmd_line): Command(cmd_line){};//, ptrPrompt(ptrPrompt){};
        virtual ~RedirectionCommand() {}
        void execute() override;
        //void prepare() override;
        //void cleanup() override;
};

///func 2 - showpid
class ShowPidCommand : public BuiltInCommand {
    //private:
        //pid_t pid;
    public:
        //ShowPidCommand(const char* cmd_line, pid_t pid): BuiltInCommand(cmd_line), pid(pid){}
        ShowPidCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
        virtual ~ShowPidCommand() = default;
        void execute() override;
};

///func 3 - pwd
class GetCurrDirCommand : public BuiltInCommand {
    public:
        GetCurrDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
        virtual ~GetCurrDirCommand() {}
        void execute() override;
};

///func 4 - cd
class ChangeDirCommand : public BuiltInCommand {
    private:
        //char** lastPwd;
        //char** currentPwd;
    public:
        ChangeDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line){}//ChangeDirCommand(const char* cmd_line, char** lastPwd = nullptr, char** currentPwd = nullptr);
        virtual ~ChangeDirCommand() {}
        void execute() override;
};

class JobsList {///it should also be created once, no?
    //private:
    public:
        class JobEntry {
            private:
                /*int jobID;*/
                /*int signal;*/
                int processID;
                std::string command;
                time_t time;
                STATUS status;
            public:
                JobEntry():processID(processID), command(command), time(time), status(status){};///to change!
                ~JobEntry() = default;
                int GetProcessID();
                std::string GetCommand();
                /*void SetSignal(int signal);*/
        };
        std::map<int, JobEntry> jobsMap;///the key is jobID
        int nextID;
    public:
        JobsList(){};
        ~JobsList() = default;
        void addJob(Command* cmd, bool isStopped = false);
        void printJobsList();
        void killAllJobs();
        void removeFinishedJobs();
        JobEntry* getJobById(int jobId);
        void removeJobById(int jobId);
        JobEntry* getLastJob(int* lastJobId);
        JobEntry* getLastStoppedJob(int *jobId);///???
        // TODO: Add extra methods or modify exisitng ones as needed
};

///func 5 - jobs
class JobsCommand : public BuiltInCommand {
        ///When we declare a member of a class as static it means no matter how many objects of the class are created, there is only one copy of the static member.
    public:
        JobsCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
        virtual ~JobsCommand() {}
        void execute() override;
};

///func 6 - kill
class KillCommand : public BuiltInCommand {
    public:
        KillCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
        virtual ~KillCommand() {}
        void execute() override;
};

///func 7 - fg
class ForegroundCommand : public BuiltInCommand {
    public:
        ForegroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
        virtual ~ForegroundCommand() {}
        void execute() override;
};

///func 8 - bg
class BackgroundCommand : public BuiltInCommand {
    public:
        BackgroundCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
        virtual ~BackgroundCommand() {}
        void execute() override;
};

///func 9 - quit
class QuitCommand : public BuiltInCommand {

    public:
        QuitCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
        virtual ~QuitCommand() {}
        void execute() override;
};

///special commands
class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
};

class CatCommand : public BuiltInCommand {
    public:
        CatCommand(const char* cmd_line);
        virtual ~CatCommand() {}
        void execute() override;
};

class SmallShell {
    // TODO: Add your data members
    private:
        pid_t pid;
        std::string lastPwd;
        std::string currentPwd;
        std::string prompt;
        JobsList jobsList;
        SmallShell(): pid(getpid()), lastPwd(""), currentPwd(""), prompt("smash"), jobsList(jobsList){}///getpid is always successful according to man
    public:
        class SmashExceptions;
        Command *CreateCommand(const char* cmd_line);
        SmallShell(SmallShell const&) = delete; // disable copy ctor
        void operator=(SmallShell const&) = delete; // disable = operator
        static SmallShell& getInstance() {// make SmallShell singleton
            static SmallShell instance; // Guaranteed to be destroyed.// Instantiated on first use.
            return instance;
        }
        ~SmallShell();
        void executeCommand(const char* cmd_line);
        //TODO: add extra methods as needed
        pid_t getPid() {return pid;}
        std::string getLastPwd() {return lastPwd;}
        std::string getCurrentPwd() {return currentPwd;}
        std::string GetPrompt();
        JobsList& getJobsList();
        void setPrompt(std::string newPrompt) {prompt = newPrompt;}
        void setLastPwd(std::string pwd) {lastPwd = pwd;}
        void setCurrentPwd(std::string pwd) {currentPwd = pwd;}
};

/*class Exceptions : public std::exception{
    public:
        Exceptions() : std::exception(){};
        ~Exceptions() override = default;
};

class SmashExceptions: public Exceptions{
    private:
        std::string what_message;
    protected:
        explicit SmashExceptions(const std::string& exception_type);///EXPLICIT
        ~SmashExceptions() override = default;
    public:
        const char* what() const noexcept override;
};

class KillInvalidArg: public SmashExceptions{
    public:
        KillInvalidArg();
        ~KillInvalidArg();
};*/


#endif //SMASH_COMMAND_H_
