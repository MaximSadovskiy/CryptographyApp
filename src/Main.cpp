#include "Application.hpp"

#pragma execution_character_set("utf-8");

int main()
{
    auto&& app = Application::createApp(650, 500, "Генератор секретных ключей");
    if (!app) return EXIT_FAILURE;

    app->runApp();

    delete(app);

    return EXIT_SUCCESS;
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    main();
}