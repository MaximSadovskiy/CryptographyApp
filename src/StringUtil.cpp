#include "StringUtil.hpp"

#include "csprng/duthomhas/csprng.hpp"
#include "Defines.hpp"
#include <random>

const std::string alphaCharacters = "abcdefghijklmnopqrstuvwxyz";
const std::string numberCharacters = "1234567890";
const std::string upperCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string specialCharacters = "!@#$%^&*()-+=\/'/\\<>:";

duthomhas::csprng rngseed;
std::mt19937 rng(rngseed.sseq);

std::string StringUtil::generatePassword(uint32_t length, bool useNumbers, bool useUpperCase, bool useSpecialChars) {
    std::string generatedPassword;
    for (uint32_t i = 0; i < length; ++i)
    {
        char ch;
        if (useNumbers && randomChance(25))
        {
            ch = getChar(numberCharacters);
        }
        else if (useUpperCase && randomChance(25))
        {
            ch = getChar(upperCharacters);
        }
        else if (useSpecialChars && randomChance(35))
        {
            ch = getChar(specialCharacters);
        }
        else {
            ch = getChar(alphaCharacters);
        }
        generatedPassword += ch;
    }
    return generatedPassword;
}
std::string StringUtil::GetLastErrorAsString()
{
    DWORD errorMessageID = GetLastError();
    if (errorMessageID == 0)
    {
        return std::string();
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    // Free the buffer allocated by FormatMessage
    LocalFree(messageBuffer);

    return message;
}
uint64_t StringUtil::padString(std::string& str, size_t size, char charToAppend) {
    uint64_t padded_size = size - str.length();
    if (size > str.length()) {
        str.append(size - str.length(), charToAppend);
    }
    return padded_size;
}
wchar_t* StringUtil::getWcharFromStr(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);
    wchar_t* res = new wchar_t[size_needed + 1];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), res, size_needed);
    res[size_needed] = '\0';
    return res;
}
std::optional<std::vector<std::string>> StringUtil::readDataFromFile(std::string filePath, uint32_t dataSize, bool isEncrypting) {
    LPDWORD lpNumberOfBytesWritten = { 0 };
    LPOVERLAPPED lpOverlapped = { 0 };
    wchar_t* path = getWcharFromStr(filePath);
    HANDLE h = CreateFile(
        path,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (h == INVALID_HANDLE_VALUE) {
        delete[] path;
        return std::nullopt;
    }
    LARGE_INTEGER size = { 0 };
    GetFileSizeEx(h, &size);
    char* buffer = new char[size.QuadPart + 1];
    if (!ReadFile(h, buffer, size.QuadPart, NULL, NULL)) {
        delete[] buffer;
        return std::nullopt;
    }
    buffer[size.QuadPart] = '\0';
    CloseHandle(h);
    std::vector<std::string> res = splitStrByBytes(std::string(buffer, size.QuadPart), size.QuadPart, dataSize, isEncrypting);

    delete[] path;
    delete[] buffer;
    return res;
}
std::vector<std::string> StringUtil::splitStrByBytes(std::string str, size_t size, size_t bytes, bool isEncrypting) {
    std::vector<std::string> result;
    size_t startPos = 0;

    while (startPos < size) {
        if (startPos == 0 && !isEncrypting) {
            result.push_back(str.substr(startPos, bytes + 8));
            startPos += bytes + 8;
            continue;
        }
        result.push_back(str.substr(startPos, bytes));
        startPos += bytes;
    }
    if (startPos < size) {
        result.back() += str.substr(startPos);
    }

    return result;
}
std::optional<std::string> StringUtil::writeDataToFile(std::string filePath, const std::vector<std::string>& data)
{
    LPDWORD lpNumberOfBytesWritten = { 0 };
    LPOVERLAPPED lpOverlapped = { 0 };
    wchar_t* path = getWcharFromStr(filePath);
    HANDLE h = CreateFile(
        path,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (h == INVALID_HANDLE_VALUE) {
        delete[] path;
        return GetLastErrorAsString();
    }
    for (const std::string& chunk : data) {
        WriteFile(h, chunk.c_str(), chunk.size(), lpNumberOfBytesWritten, lpOverlapped);
    }
    CloseHandle(h);
    delete[] path;
    return std::nullopt;
}
bool StringUtil::randomChance(uint8_t chancePercent)
{
    return rng() % 101 <= chancePercent;
}
char StringUtil::getChar(const std::string& str)
{
    return str[rng() % str.size()];
}
#ifdef OS_WINDOWS
std::string StringUtil::wstringToStr(const std::wstring& str) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &utf8_str[0], size_needed, NULL, NULL);
    return utf8_str;
}
#else 
std::string StringUtil::wstringToStr(const std::wstring& str) {
    std::string utf8_str;
    for (wchar_t wchar : str) {
        if (wchar <= 0x7f) {
            utf8_str.push_back(static_cast<char>(wchar));
        }
        else if (wchar <= 0x7ff) {
            utf8_str.push_back(0xc0 | (wchar >> 6));
            utf8_str.push_back(0x80 | (wchar & 0x3f));
        }
        else if (wchar <= 0xffff) {
            utf8_str.push_back(0xe0 | (wchar >> 12));
            utf8_str.push_back(0x80 | ((wchar >> 6) & 0x3f));
            utf8_str.push_back(0x80 | (wchar & 0x3f));
        }
        else if (wchar <= 0x10ffff) {
            utf8_str.push_back(0xf0 | (wchar >> 18));
            utf8_str.push_back(0x80 | ((wchar >> 12) & 0x3f));
            utf8_str.push_back(0x80 | ((wchar >> 6) & 0x3f));
            utf8_str.push_back(0x80 | (wchar & 0x3f));
        }
        else {
            // Invalid unicode character
            return std::string();
        }
    }
    return utf8_str;
}
#endif