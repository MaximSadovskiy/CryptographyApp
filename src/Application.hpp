#pragma once

#include "glew/glew.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "glfw/glfw3.h"
#include "encrypt/Encryptions.h"

#include <iostream>
#include <optional>
#include <filesystem>

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define OS_WINDOWS
#elif __linux__
	#define OS_LINUX
#endif
#define UTF8_TEXT(str) ((const char*)u8##str)
#ifdef OS_WINDOWS
	#include <Windows.h>
#endif

class Application
{
public:
	static Application* createApp(int32_t width, int32_t height, const char* title);
	virtual ~Application();
	void runApp();
	inline void stopApp();
	const char* getGlslVersion();

	const int32_t windowWidth;
	const int32_t windowHeigth;
	const char const* windowTitle;

private:
	Application(int32_t width, int32_t height, const char* title);
	inline bool initilizeGraphics();
	inline void initilizeImgui();
	inline void changeFont();
	inline std::optional<std::vector<std::string>> readDataFromFile(std::string filePath, uint32_t dataSize, bool isEncrypting);
	inline std::optional<std::string> writeDataToFile(std::string filePath, const std::vector<std::string>& data);
	inline std::optional<std::string> cryptFile(bool isEncrypting);
	inline SymAlg::Ptr getAlgoritm() const;
	inline size_t getAlgoritmSize() const;
	std::string generatePassword(uint32_t length);
	void loadTexture(const char* path, unsigned int& textId);
	std::string wstringToStr(const std::wstring& str) const;

	bool isRunning_		             = false;
	bool useNumbers_                 = true;
	bool useUpperCase_               = true;
	bool useSpecialChars_            = true;
	GLFWwindow* glfwWindow_          = NULL;
	std::string algoritmName_	     = "AES";
	std::string inputFilePath_       = {0};
	std::string inputKey_            = {0};
	unsigned int folderTextureID_ = 0;
	unsigned int keyTextureID_ = 0;
	float compressionRate_ = 0.0f;
};
template <typename T>
struct Expected
{
	std::unique_ptr<T> val;
	std::string error;
};