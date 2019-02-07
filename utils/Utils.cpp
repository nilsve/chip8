//
// Created by Nils van Eijk on 20-02-18.
//

#include "Utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <vector>

namespace chip8 {
    namespace utils {

        using namespace std;

        std::vector<unsigned char> Utils::readChip8Bin(const std::wstring &fileName) {
            ifstream file(Utils::wstring_to_utf8(fileName).c_str(), std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                Utils::throwError(L"Couldn't open file " + fileName + L": " + utf8_to_wstring(strerror(errno)));
            }

            file.seekg(0,ios_base::end);
            unsigned int size = file.tellg();
            file.seekg(0);

            std::vector<unsigned char> result;
            result.resize(size);
            file.read(reinterpret_cast<char*>(&result[0]), size);


            return std::move(result);
        }

        std::string Utils::wstring_to_utf8(const std::wstring &str) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
            return myconv.to_bytes(str);
        }

        std::wstring Utils::utf8_to_wstring(const std::string &str) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
            return myconv.from_bytes(str);
        }

        bool Utils::stringReplace(std::wstring &str, const std::wstring &from, const std::wstring &to) {
            size_t start_pos = str.find(from);
            if (start_pos == std::wstring::npos)
                return false;
            str.replace(start_pos, from.length(), to);
            return true;
        }

        std::wstring Utils::generateUuid(unsigned int length) {
            static std::wstring characters = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
            wstring output;

            for (unsigned int i = 0; i < length; i++) {
                output += characters[rand() % characters.length()];
            }

            return output;
        }

        void Utils::throwError(std::wstring message) {
            return throwError(Utils::wstring_to_utf8(message));
        }

        void Utils::throwError(std::string message) {
            throw std::invalid_argument(message.c_str());
        }

        void Utils::Log(const std::string &message) {
            return Log(Utils::utf8_to_wstring(message));
        }

        void Utils::Log(const std::wstring &message) {
            std::wcout << message;
        }
    }
}