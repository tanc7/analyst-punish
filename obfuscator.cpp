#include <iostream>
#include <string>
#include <vector>
#define KEY '726a628080d2f59f210cea4860c0eefb5bc5110fc1a70cb538595bcb487aefac'
using namespace std;
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

Obfuscator obs;
string b64obfuscate(string s, char key) {
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