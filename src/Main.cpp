#include "Application.hpp"
#include <optional>

#pragma execution_character_set("utf-8");

int main()
{
    auto&& app = Application::createApp(650, 500, "��������� ��������� ������");
    if (!app) return EXIT_FAILURE;

    app->startApp();

    return EXIT_SUCCESS;
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    main();
}