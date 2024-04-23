#include "Application.hpp"
#include "encrypt/Encryptions.h"
#include "nativefiledialog/nfd.h"
#include "csprng/duthomhas/csprng.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <string>
#include <random>
#include <fstream>
#include <iomanip>

const std::string alphaCharacters   = "abcdefghijklmnopqrstuvwxyz";
const std::string numberCharacters  = "1234567890";
const std::string upperCharacters   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string specialCharacters = "!@#$%^&*()-+=\/'/\\<>:";

duthomhas::csprng rngseed;
std::mt19937 rng(rngseed.sseq);

Application::Application(int32_t width, int32_t height, const char* title)
	: windowWidth(width), windowHeigth(height), windowTitle(title)
{
    inputFilePath_.reserve(256);
    inputKey_.reserve(64);
}
Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfwWindow_);
    glfwTerminate();
}
Application* Application::createApp(int32_t width = 500, int32_t height = 500, const char* title = "Default window")
{
    auto* app = new Application(width, height, title);
	if (!app->initilizeGraphics()) return nullptr;
	return app;
}
bool randomChance(uint8_t chancePercent) {
    return rng() % 101 <= chancePercent;
}
char getChar(const std::string& str) {
    return str[rng() % str.size()];
}
std::string Application::generatePassword(uint32_t length) {
    std::string generatedPassword;
    for (int i = 0; i < length; i++) {
        char ch;
        if (useNumbers_ && randomChance(25)) {
            ch = getChar(numberCharacters);
        }
        else if (useUpperCase_ && randomChance(25)) {
            ch = getChar(upperCharacters);
        }
        else if (useSpecialChars_ && randomChance(35)) {
            ch = getChar(specialCharacters);
        }
        else {
            ch = getChar(alphaCharacters);
        }
        generatedPassword += ch;
    }
    return generatedPassword;
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

    this->initilizeImgui();
    return true;
}

const char* listbox_items[] = {"AES", "SKIPJACK", "TWOFISH", "DES", "SEED"};
int listbox_items_current = 0;
bool show_about = false;

void Application::runApp()
{
    this->isRunning_ = true;
    std::string error = "";
    auto& io = ImGui::GetIO();
    
    while (!glfwWindowShouldClose(glfwWindow_) && isRunning_) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::Begin("MainMenu", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu(UTF8_TEXT("Настройки"))) {
                if (ImGui::MenuItem(UTF8_TEXT("О программе"), NULL)) {
                    show_about = true;
                }
                if (ImGui::MenuItem(UTF8_TEXT("Закрыть приложение"), NULL)) {
                    isRunning_ = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (show_about) {
            ImGui::OpenPopup(UTF8_TEXT("О программе"));
        }
        if (ImGui::BeginPopupModal(UTF8_TEXT("О программе"))) {
            ImGui::Text(UTF8_TEXT("Генератор секретных ключей.\nРазработан как пример для дипломной работы."));
            if (ImGui::Button(UTF8_TEXT("Закрыть"))) {
                ImGui::CloseCurrentPopup();
                show_about = false;
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal(UTF8_TEXT("Успех"))) {
            ImGui::Text(UTF8_TEXT("Файл \"testdata.txt\" был успешно зашифрован."));
            if (ImGui::Button(UTF8_TEXT("Закрыть"))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::Indent(270);
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
        ImGui::PushStyleColor(ImGuiCol_Text, {0.75f, 0.7f, 0.1f, 1.f});
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
            inputKey_ = generatePassword(getAlgoritmSize());
            ImGui::SetClipboardText(inputKey_.c_str());
        }
        ImGui::Spacing();
        ImGui::Indent(25);
        if (ImGui::Button(UTF8_TEXT("Зашифровать"))) {
            std::optional<std::string> res = cryptFile(true);
            if (res.has_value()) {
                error = res.value();
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
                error = res.value();
                ImGui::OpenPopup(UTF8_TEXT("Ошибка"));
            }
        }
        if (ImGui::BeginPopupModal(UTF8_TEXT("Ошибка"))) {
            ImGui::Text(error.c_str());
            if (ImGui::Button(UTF8_TEXT("Закрыть"))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::Indent(-25);
        ImGui::InvisibleButton("##padded-text", ImVec2(15, 15));
        ImGui::Indent(105);
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

        ImGui::End();
        ImGui::Render();

        //opengl render
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(this->glfwWindow_);
    }
}
void Application::initilizeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(this->glfwWindow_, true);
    ImGui_ImplOpenGL3_Init(this->getGlslVersion());
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
        MessageBoxW(NULL, L"Не удалось найти необходимы шрифт в папке resoureces\nВозможно русский текст не будет корректно отображаться", L"Важно", MB_OK);
    }
}
std::vector<std::string> split_str_by_bytes(std::string str, size_t size, size_t bytes, bool isEncrypting) {
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
//TODO: ДОДелать интерфейс
uint64_t pad_string(std::string& str, size_t size, char c) {
    uint64_t padded_size = size - str.length();
    if (size > str.length()) {
        str.append(size - str.length(), c);
    }
    return padded_size;
}
std::string GetLastErrorAsString()
{
    DWORD errorMessageID = GetLastError();
    if (errorMessageID == 0)
    {
        return std::string(); // No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    // Free the buffer allocated by FormatMessage
    LocalFree(messageBuffer);

    return message;
}
wchar_t* getWcharFromStr(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);
    wchar_t* res = new wchar_t[size_needed+1];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), res, size_needed);
    res[size_needed] = '\0';
    return res;
}
inline std::optional<std::vector<std::string>> Application::readDataFromFile(std::string filePath, uint32_t dataSize, bool isEncrypting) {
    LPDWORD lpNumberOfBytesWritten = {0};
    LPOVERLAPPED lpOverlapped = {0};
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
    LARGE_INTEGER size = {0};
    GetFileSizeEx(h, &size);
    char* buffer = new char[size.QuadPart + 1];
    if (!ReadFile(h, buffer, size.QuadPart, NULL, NULL)) {
        delete[] buffer;
        return std::nullopt;
    }
    buffer[size.QuadPart] = '\0';
    CloseHandle(h);
    std::vector<std::string> res = split_str_by_bytes(std::string(buffer, size.QuadPart), size.QuadPart, dataSize, isEncrypting);

    delete[] path;
    delete[] buffer;
    return res;
}
inline std::optional<std::string> Application::writeDataToFile(std::string filePath, const std::vector<std::string>& data)
{
    LPDWORD lpNumberOfBytesWritten = {0};
    LPOVERLAPPED lpOverlapped = {0};
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
inline std::optional<std::string> Application::cryptFile(bool isEncrypting)
{
    std::shared_ptr<SymAlg> alg = getAlgoritm();
    std::string newFilePath = inputFilePath_;
    std::vector<std::string> data_vector;
    if (isEncrypting) {
        newFilePath += "_encrypted.txt";
    } else {
        newFilePath += "_decrypted.txt";
    }
    uint32_t key_size = getAlgoritmSize();
    uint32_t data_size = alg->datasize();
    if (inputKey_.size() != key_size) {
        inputKey_ = generatePassword(key_size);
    }
    alg->setkey(inputKey_);
    auto&& split_data = readDataFromFile(inputFilePath_, data_size, isEncrypting).value_or(std::vector<std::string>());
    uint64_t padded_size = 0;
    std::string padded_info = "";
    if (!isEncrypting && split_data.size() > 0) {
        padded_info = split_data[0].substr(0, 8);
        split_data[0] = split_data[0].substr(8, split_data[0].size());
    }
    for (auto it = split_data.begin(); it != split_data.end(); ++it) {
        try {
            if (it == std::prev(split_data.end()) && it->size() < data_size && isEncrypting) {
                padded_size = pad_string(*it, data_size, -1);
            }
            std::string data;
            if (isEncrypting) {
                data = alg->encrypt(*it);
            } else {
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
        char buffer[9];
        sprintf(buffer, "%08lu", padded_size);
        buffer[8] = '\0';
        data_vector[0] = firstStr.insert(0, buffer);
    } else {
        if (data_vector.size() > 0) {
            sscanf(padded_info.c_str(), "%lu", &padded_size);
            //remove padding
            std::string lastStr = data_vector[data_vector.size() - 1];
            data_vector[data_vector.size() - 1] = lastStr.substr(0, lastStr.size() - padded_size);
        }
    }
    return writeDataToFile(newFilePath, data_vector);
}
inline SymAlg::Ptr Application::getAlgoritm() const
{
    if (algoritmName_ == "DES") {
        return std::make_shared<AES>();
    }
    else if (algoritmName_ == "TWOFISH") {
        return std::make_shared<Twofish>();
    }
    else if (algoritmName_ == "SEED") {
        return std::make_shared<SEED>();
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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #endif
    return glsl_version;
}
#ifdef OS_WINDOWS
std::string Application::wstringToStr(const std::wstring& str) const {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &utf8_str[0], size_needed, NULL, NULL);
    return utf8_str;
}
#else 
std::string Application::wstringToStr(const std::wstring& str) const {
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