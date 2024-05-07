#pragma once

#include "glew/glew.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "glfw/glfw3.h"
#include "Defines.hpp"
#include "encrypt/Encryptions.h"

#include <iostream>
#include <optional>
#include <filesystem>
#include <memory>

class Application
{
public:
	// App supports Unicode text
	static Application* createApp(int32_t width, int32_t height, const char* title);
	void startApp();
	inline void stopApp();
	static const char* getGlslVersion();
	virtual ~Application();
public:
	static const uint32_t maxPaddedSize = 8;
	// Theres is no point in resize in this app.
	const int32_t windowWidth;
	const int32_t windowHeigth;
	const char const* windowTitle;
private:
	Application(int32_t width, int32_t height, const char* title);
	inline bool initilizeGraphics();
	inline bool initilizeImgui();
	inline std::optional<std::string> cryptFile(bool isEncrypting);
	inline SymAlg::Ptr getAlgoritm() const;
	inline size_t getAlgoritmSize() const;
	void loadTexture(const char* path, unsigned int& textId);

	void drawTopBar();
	void drawMainMenu();
	void drawPasswordSettingsMenu();
private:
	bool isRunning_		             = false;
	bool showAbout_					 = false;
	bool useNumbers_                 = true;
	bool useUpperCase_               = true;
	bool useSpecialChars_            = true;
	GLFWwindow* glfwWindow_          = NULL;
	std::string algoritmName_	     = "AES";
	std::string inputFilePath_       = {0};
	std::string inputKey_            = {0};
	std::string errorStr_			 = {0};
	float compressionRate_ = 0.0f;
	unsigned int folderTextureID_ = 0;
	unsigned int keyTextureID_ = 0;
};
