#include <PureWebGUI/PureWebGUI.hpp>
#include <cstdio>
#include <string>

extern "C" __declspec(dllexport) int RunPureWebGUIDemo() {
    if (!pure::Create("Pure WebGUI DLL", 640, 420)) {
        return 1;
    }

    bool enabled = true;
    std::string message = "Loaded from a DLL";

    while (pure::Running()) {
        pure::Begin("Pure WebGUI DLL");
        pure::Text("This window was created by a DLL entry point.");
        pure::Separator();
        pure::Checkbox("Enable feature", &enabled);
        pure::InputText("Message", &message);

        if (pure::Button("Write to host console")) {
            std::printf("DLL message: %s (enabled: %s)\n", message.c_str(), enabled ? "yes" : "no");
        }

        pure::End();
        pure::Render();
    }

    pure::Shutdown();
    return 0;
}
