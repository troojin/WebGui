#include <PureWebGUI/PureWebGUI.hpp>
#include <cstdio>
#include <string>

int main() {
    if (!pure::Create("Widget Gallery", 720, 480)) {
        std::fprintf(stderr, "Failed to initialize Pure WebGUI.\n");
        return 1;
    }

    bool dark_mode = true;
    std::string project_name = "Pure WebGUI";

    while (pure::Running()) {
        pure::Begin("Widget Gallery");
        pure::Text("Widget gallery");
        pure::Separator();

        pure::InputText("Project name", &project_name);
        pure::Checkbox("Use dark mode", &dark_mode);

        if (pure::Button("Save project", "save-project")) {
            std::printf("Saving %s (dark mode: %s)\n", project_name.c_str(), dark_mode ? "on" : "off");
        }

        pure::Text(("Current project: " + project_name).c_str());
        pure::End();
        pure::Render();
    }

    pure::Shutdown();
    return 0;
}
