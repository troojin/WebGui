#include <PureWebGUI/PureWebGUI.hpp>
#include <cstdio>
#include <string>

int main() {
    if (!pure::Create("Pure WebGUI", 1024, 768)) {
        std::fprintf(stderr, "Failed to initialize Pure WebGUI.\n");
        return 1;
    }

    int clicks = 0;
    bool notifications = true;
    std::string name = "Ada";

    while (pure::Running()) {
        pure::Begin("Pure WebGUI");

        pure::Text("A small native C++ app rendered in WebView2.");
        pure::Separator();

        if (pure::Button("Click Me")) {
            ++clicks;
            std::printf("C++ backend says: Button was clicked! (Total: %d)\n", clicks);
        }

        pure::Text(("Button clicks: " + std::to_string(clicks)).c_str());

        if (pure::Checkbox("Enable notifications", &notifications)) {
            std::printf("Notifications: %s\n", notifications ? "enabled" : "disabled");
        }

        if (pure::InputText("Your name", &name)) {
            std::printf("Name changed to: %s\n", name.c_str());
        }

        pure::Text(("Hello, " + name + "!").c_str());

        pure::End();
        pure::Render();
    }

    pure::Shutdown();
    return 0;
}
