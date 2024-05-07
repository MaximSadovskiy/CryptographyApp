#pragma once

#include <string>
#include <optional>
#include <vector>

class StringUtil
{
public:
	static std::string generatePassword(uint32_t length, bool useNumbers, bool useUpperCase, bool useSpecialChars);
	static std::string GetLastErrorAsString();
	static std::optional<std::vector<std::string>> readDataFromFile(std::string filePath, uint32_t dataSize, bool isEncrypting);
	static std::optional<std::string> writeDataToFile(std::string filePath, const std::vector<std::string>& data);
	static uint64_t padString(std::string& str, size_t size, char charToAppend);
private:
	static bool randomChance(uint8_t chancePercent);
	static char getChar(const std::string& str);
	static std::string wstringToStr(const std::wstring& str);
	static wchar_t* getWcharFromStr(const std::string& str);
	static std::vector<std::string> splitStrByBytes(std::string str, size_t size, size_t bytes, bool isEncrypting);
	StringUtil();
	~StringUtil();
};

