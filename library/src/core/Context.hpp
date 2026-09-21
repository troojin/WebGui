#pragma once

#include <WebGUI/WebGUI.hpp>

#include <memory>

class Win32Backend;

class WebGUI::Window::Impl {
public:
    Impl(const char* title, int width, int height);

    bool valid() const;
    bool load(const std::string& file);
    void show();
    bool poll_events();
    void close();
    void post_message(const std::string& message);
    void execute_script(const std::string& script);
    void on_message(MessageHandler handler);

private:
    std::unique_ptr<Win32Backend> m_backend;
    MessageHandler m_handler;
};
