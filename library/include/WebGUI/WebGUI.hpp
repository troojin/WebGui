#pragma once

#include <functional>
#include <memory>
#include <string>

namespace WebGUI {

class Window {
public:
    using MessageHandler = std::function<void(const std::string& message)>;

    Window(const char* title, int width = 1024, int height = 768);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    bool valid() const;
    bool load(const std::string& file);
    void show();
    bool poll_events();
    void run();
    void close();

    void post_message(const std::string& message);
    void execute_script(const std::string& script);
    void on_message(MessageHandler handler);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace WebGUI
