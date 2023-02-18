#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <linux/limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <sys/ptrace.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
class Antidebug {
public:
//    ptrace check, segfaults if it finds ptrace
    void ptc() {
        if(ptrace(PTRACE_TRACEME)) {
            printf("Debugger detected! Can't steal my tickets suckah!\n");
            raise(SIGSEGV);
        } else {
            return;
        }
    }
//    Check for common debuggers, segfault if it finds any
    void dpc() {
        if (isUnderDebugger())
        {std::cout << "I am being debugged. Ticketmeister is being DDoSed!!!!\n" << std::endl;
            raise(SIGSEGV);}
        else
            return;
    }
//    Attach self in debugging mode, if cannot attach, segfault. Pretty unreliable, just like timing attacks.
    void sdc() {
        if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
            std::cout << "YOU SHOULD HAVE PATCHED ME OUT, SCALPER!!!\n" << std::endl;
            raise(SIGSEGV);
        } else {
            return;}


    }
private:
    bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }
    bool isUnderDebugger()
    {
        bool result = false;
        /*
         * /proc/self/stat has PID of parent process as fourth parameter.
         */
        std::string stat;
        std::ifstream file("/proc/self/stat");

        for(int i = 0; i < 4; ++i)
            file >> stat;

        std::string parent_path = std::string("/proc/") + stat + "/exe";
        char path[PATH_MAX + 1];
        memset(path, 0, PATH_MAX + 1);
        readlink(parent_path.c_str(), path, PATH_MAX);

        std::vector<std::string> debuggers = {"gdb", "lldb-server"};

        for (auto &p: debuggers)
        {
            if (hasEnding(std::string(path), p))
            {
                result = true;
                break;
            }
        }

        return result;
    }
};
