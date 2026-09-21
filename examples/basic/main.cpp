#include <WebGUI/WebGUI.hpp>

#include <iostream>

int main() {
    WebGUI::Window window("WebGUI", 1024, 768);
    if (!window.valid() || !window.load("web/index.html")) {
        std::cerr << "Failed to create WebGUI window.\n";
        return 1;
    }

    window.on_message([&window](const std::string& message) {
        std::cout << "JavaScript: " << message << '\n';
        window.post_message("Message received by C++.");
    });
    window.show();
    window.run();
}
