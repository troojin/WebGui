#include <WebGUI/WebGUI.hpp>

#include <windows.h>

#include <iostream>

namespace {

LRESULT CALLBACK HostWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wparam, lparam);
}

}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int command_show) {
    const wchar_t class_name[] = L"WebGUIEmbeddedExample";
    WNDCLASSW window_class = {};
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.lpfnWndProc = HostWindowProc;
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&window_class);

    HWND host = CreateWindowExW(0, class_name, L"WebGUI embedded example", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1100, 700, nullptr, nullptr, instance, nullptr);
    if (!host) return 1;
    ShowWindow(host, command_show);

    WebGUI::Context gui;
    if (!gui.attach(host) || !gui.load("web/index.html")) {
        std::cerr << "Failed to attach WebGUI.\n";
        DestroyWindow(host);
        return 1;
    }
    gui.on_message([&gui](const std::string& message) {
        std::cout << "JavaScript: " << message << '\n';
        gui.post_message("C++ received your message");
    });

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return 0;
}
