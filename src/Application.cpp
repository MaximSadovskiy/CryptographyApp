#include "Application.hpp"
#include "encrypt/Encryptions.h"
#include "nativefiledialog/nfd.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Defines.hpp"
#include "StringUtil.hpp"

#include <string>
#include <fstream>
#include <iomanip>

const char* listbox_items[] = { "AES", "SKIPJACK", "TWOFISH", "DES", "SEED" };
int listbox_items_current = 0;
bool showAbout_ = false;

Application::Application(int32_t width, int32_t height, const char* title)
	: windowWidth(width), windowHeigth(height), windowTitle(title)
{
    inputFilePath_.reserve(256);
    inputKey_.reserve(64);
}
Application* Application::createApp(int32_t width = 500, int32_t height = 500, const char* title = "Default window")
{
    auto&& app = new Application(width, height, title);
	if (!app->initilizeGraphics()) return nullptr;
    return app;
}

bool Application::initilizeGraphics()
{
    if (!glewInit()) {
        MessageBoxW(NULL, L"Не удалось инициализировать glew!\n", L"Критическая ошибка", MB_OK);
        return false;
    }
    if (glfwInit() != GLFW_TRUE) {
        MessageBoxW(NULL, L"Не удалось инициализировать glfw!\n", L"Критическая ошибка", MB_OK);
        return false;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int monitorWidth = mode->width;
    int monitorHeight = mode->height;

    int windowX = (monitorWidth - windowWidth) / 2;
    int windowY = (monitorHeight - windowHeigth) / 2;
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    this->glfwWindow_ = glfwCreateWindow(windowWidth, windowHeigth, windowTitle, NULL, NULL);
    if (!this->glfwWindow_) {
        MessageBoxW(NULL, L"Не удалось инициализировать окно!\n", L"Критическая ошибка", MB_OK);
        return false;
    }
    glfwMakeContextCurrent(this->glfwWindow_);
    glfwSetWindowPos(this->glfwWindow_, windowX, windowY);
    glfwSwapInterval(1); // vsync

    GLFWimage images[1] = {0};
    images[0].pixels = stbi_load("resources/icons/title_icon.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(glfwWindow_, 1, images);
    stbi_image_free(images[0].pixels);

    loadTexture("resources/icons/folder.png", folderTextureID_);
    loadTexture("resources/icons/generate_key.png", keyTextureID_);

    return this->initilizeImgui();
}
void Application::startApp()
{
    this->isRunning_ = true;
    const ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_MenuBar |
          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoCollapse;
    
    while (!glfwWindowShouldClose(glfwWindow_) && isRunning_) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowPos({ 0, 0 });

        ImGui::Begin("MainMenu", NULL, mainWindowFlags);
        
        drawTopBar();

        drawMainMenu();

        drawPasswordSettingsMenu();

        ImGui::End();
        ImGui::Render();

        //opengl render
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(this->glfwWindow_);
    }
    this->isRunning_ = false;
}
bool Application::initilizeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    assert(glfwWindow_ != NULL);
    if (!ImGui_ImplGlfw_InitForOpenGL(this->glfwWindow_, true)) {
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init(Application::getGlslVersion())) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGuiStyle& style = ImGui::GetStyle();
    colors[ImGuiCol_WindowBg]              =  ImVec4(0.10f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_Button]                =  ImVec4(0.05f, 0.92f, 0.05f, 0.38f);
    colors[ImGuiCol_ScrollbarBg]           =  ImVec4(0.40f, 0.62f, 0.80f, 0.15f);
    colors[ImGuiCol_ScrollbarGrab]         =  ImVec4(0.39f, 0.64f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered]  =  ImVec4(0.28f, 0.67f, 0.80f, 0.59f);
    colors[ImGuiCol_ScrollbarGrabActive]   =  ImVec4(0.25f, 0.48f, 0.53f, 0.67f);
    colors[ImGuiCol_SliderGrabActive]      =  ImVec4(0.31f, 0.97f, 0.19f, 1.00f);
    colors[ImGuiCol_SliderGrab]            =  ImVec4(0.31f, 0.97f, 0.19f, 1.00f);
    colors[ImGuiCol_CheckMark]             =  ImVec4(0.05f, 0.92f, 0.05f, 0.58f);
    
    ImFont* font = io.Fonts->AddFontFromFileTTF("resources/fonts/ArimoBold.ttf", 21.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    if (!font) {
        MessageBoxW(NULL, L"Не удалось найти необходимые шрифт в папке resources\nВозможно русский текст не будет корректно отображаться", L"Важно", MB_OK);
    }

    return true;
}
void Application::drawTopBar() {
    assert(glfwWindow_ != NULL);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu(UTF8_TEXT("Настройки"))) {
            if (ImGui::MenuItem(UTF8_TEXT("О программе"), NULL)) {
                showAbout_ = true;
            }
            if (ImGui::MenuItem(UTF8_TEXT("Закрыть приложение"), NULL)) {
                isRunning_ = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if (showAbout_) {
        ImGui::OpenPopup(UTF8_TEXT("О программе"));
    }
    if (ImGui::BeginPopupModal(UTF8_TEXT("О программе"))) {
        ImGui::Text(UTF8_TEXT("Генератор секретных ключей.\nРазработан как прототип для дипломной работы."));
        if (ImGui::Button(UTF8_TEXT("Закрыть"))) {
            ImGui::CloseCurrentPopup();
            showAbout_ = false;
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal(UTF8_TEXT("Успех"))) {
        ImGui::Text(UTF8_TEXT("Файл был успешно зашифрован."));
        if (ImGui::Button(UTF8_TEXT("Закрыть"))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::Indent(270);
}
void Application::drawMainMenu() {
    assert(glfwWindow_ != NULL);
    ImGui::Text(UTF8_TEXT("Алгоритм:"));
    ImGui::Indent(-20);
    ImGui::PushItemWidth(140);
    if (ImGui::Combo("##algoritms", &listbox_items_current, listbox_items, IM_ARRAYSIZE(listbox_items))) {
        algoritmName_ = listbox_items[listbox_items_current];
    };
    ImGui::PopItemWidth();
    ImGui::Indent(-120);
    ImGui::Text(UTF8_TEXT("Путь до файла:"));
    ImGui::PushItemWidth(390);
    ImGui::PushStyleColor(ImGuiCol_Text, { 0.75f, 0.7f, 0.1f, 1.f });
    ImGui::InputText("##file_path", inputFilePath_.data(), 256);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::ImageButton("##Folder_icon", (ImTextureID)folderTextureID_, ImVec2(25, 25))) {
        char* outPath = NULL;
        nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
        if (result == NFD_OKAY) {
            inputFilePath_ = outPath;
            ImGui::SetClipboardText(outPath);
            free(outPath);
        }
    }
    ImGui::Text(UTF8_TEXT("Ключ шифрования:"));
    ImGui::PushItemWidth(390);
    ImGui::PushStyleColor(ImGuiCol_Text, { 0.82f, 0.78f, 0.0f, 1.f });
    ImGui::InputText("##key_val", inputKey_.data(), 64);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::ImageButton("##Generate_key_icon", (ImTextureID)keyTextureID_, ImVec2(25, 25))) {
        inputKey_ = StringUtil::generatePassword(getAlgoritmSize(), useNumbers_, useUpperCase_, useSpecialChars_);
        ImGui::SetClipboardText(inputKey_.c_str());
    }
    ImGui::Spacing();
    ImGui::Indent(25);
    if (ImGui::Button(UTF8_TEXT("Зашифровать"))) {
        std::optional<std::string> res = cryptFile(true);
        if (res.has_value()) {
            errorStr_ = res.value();
            ImGui::OpenPopup(UTF8_TEXT("Ошибка"));
        }
        else {
            ImGui::OpenPopup(UTF8_TEXT("Успех"));
        }
    }
    ImGui::SameLine();
    ImGui::InvisibleButton("##padded-text", ImVec2(15, 15));
    ImGui::SameLine();
    if (ImGui::Button(UTF8_TEXT("Расшифровать"))) {
        std::optional<std::string> res = cryptFile(false);
        if (res.has_value()) {
            errorStr_ = res.value();
            ImGui::OpenPopup(UTF8_TEXT("Ошибка"));
        }
    }
    if (ImGui::BeginPopupModal(UTF8_TEXT("Ошибка"))) {
        ImGui::Text(errorStr_.c_str());
        if (ImGui::Button(UTF8_TEXT("Закрыть"))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::Indent(-25);
    ImGui::InvisibleButton("##padded-text", ImVec2(15, 15));
    ImGui::Indent(105);
}
void Application::drawPasswordSettingsMenu() {
    assert(glfwWindow_ != NULL);
    ImGui::Text(UTF8_TEXT("Уровень компресии:"));
    ImGui::Indent(-50);
    ImGui::PushItemWidth(290);
    ImGui::SliderFloat("##slider1", &compressionRate_, 0., 1., "");
    ImGui::PopItemWidth();
    ImGui::Indent(-55);
    ImGui::Indent(120);
    ImGui::InvisibleButton("##padded-text", ImVec2(15, 15));
    ImGui::Text(UTF8_TEXT("Настройки ключа:"));
    ImGui::Indent(-120);
    ImGui::Checkbox(UTF8_TEXT("Верхний регистр"), &useUpperCase_);
    ImGui::Checkbox(UTF8_TEXT("Цифры"), &useNumbers_);
    ImGui::Checkbox(UTF8_TEXT("Специальные символы"), &useSpecialChars_);
}
inline std::optional<std::string> Application::cryptFile(bool isEncrypting)
{
    std::shared_ptr<SymAlg> alg = getAlgoritm();
    std::string newFilePath = inputFilePath_;
    std::vector<std::string> data_vector;
    if (isEncrypting) {
        newFilePath += "_encrypted.txt";
    }
    else {
        newFilePath += "_decrypted.txt";
    }
    uint32_t key_size = getAlgoritmSize();
    uint32_t data_size = alg->datasize();
    if (inputKey_.size() != key_size) {
        inputKey_ = StringUtil::generatePassword(key_size, useNumbers_, useUpperCase_, useSpecialChars_);
    }
    alg->setkey(inputKey_);
    auto&& split_data = StringUtil::readDataFromFile(inputFilePath_, data_size, isEncrypting).value_or(std::vector<std::string>());
    uint64_t padded_size = 0;
    std::string padded_info = "";
    if (!isEncrypting && split_data.size() > 0) {
        padded_info = split_data[0].substr(0, Application::maxPaddedSize);
        split_data[0] = split_data[0].substr(Application::maxPaddedSize, split_data[0].size());
    }
    for (auto it = split_data.begin(); it != split_data.end(); ++it) {
        try {
            if (it == std::prev(split_data.end()) && it->size() < data_size && isEncrypting) {
                padded_size = StringUtil::padString(*it, data_size, -1);
            }
            std::string data;
            if (isEncrypting) {
                data = alg->encrypt(*it);
            }
            else {
                data = alg->decrypt(*it);
            }
            data_vector.push_back(data);
        }
        catch (const std::exception& e) {
            return e.what();
        }
    }
    if (isEncrypting) {
        if (data_vector.size() == 0) data_vector.push_back("");
        std::string firstStr = data_vector[0];
        char buffer[Application::maxPaddedSize + 1];
        sprintf(buffer, "%08lu", padded_size);
        buffer[Application::maxPaddedSize] = '\0';
        data_vector[0] = firstStr.insert(0, buffer);
    }
    else {
        if (data_vector.size() > 0) {
            sscanf(padded_info.c_str(), "%lu", &padded_size);
            //remove padding
            std::string lastStr = data_vector[data_vector.size() - 1];
            data_vector[data_vector.size() - 1] = lastStr.substr(0, lastStr.size() - padded_size);
        }
    }
    return StringUtil::writeDataToFile(newFilePath, data_vector);
}
inline SymAlg::Ptr Application::getAlgoritm() const
{
    if (algoritmName_ == "TWOFISH") {
        return std::make_shared<Twofish>();
    }
    else if (algoritmName_ == "SEED") {
        return std::make_shared<SEED>();
    }
    else if (algoritmName_ == "DES") {
        return std::make_shared<AES>();
    }
    else if (algoritmName_ == "SKIPJACK") {
        return std::make_shared<Skipjack>();
    }
    else {
        return std::make_shared<AES>();
    }
}
inline size_t Application::getAlgoritmSize() const
{
    return getAlgoritm()->blocksize() / 8;
}
void Application::loadTexture(const char* path, unsigned int& textId)
{
    int width, height, channels;
    unsigned char* image_data = stbi_load(path, &width, &height, &channels, 4);
    glGenTextures(1, &textId);
    glBindTexture(GL_TEXTURE_2D, textId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
}
Application::~Application()
{
    if (!glfwWindow_) return;

    isRunning_ = false;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfwWindow_);
    glfwTerminate();
}
void Application::stopApp()
{
    this->isRunning_ = false;
}
const char* Application::getGlslVersion()
{
    // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
    #endif
    return glsl_version;
}