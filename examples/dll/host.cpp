#include <windows.h>

#include <cstdio>
#include <thread>

using RunGuiFn = int (*)();

int main() {
    HMODULE plugin = LoadLibraryW(L"pure_webgui_plugin.dll");
    if (!plugin) {
        std::fprintf(stderr, "Could not load pure_webgui_plugin.dll.\n");
        return 1;
    }

    auto run_gui = reinterpret_cast<RunGuiFn>(GetProcAddress(plugin, "RunPureWebGUIDemo"));
    if (!run_gui) {
        std::fprintf(stderr, "Could not find RunPureWebGUIDemo.\n");
        FreeLibrary(plugin);
        return 1;
    }

    std::thread gui_thread([run_gui] { run_gui(); });
    gui_thread.join();
    FreeLibrary(plugin);
    return 0;
}
