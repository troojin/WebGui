#include "Context.hpp"
#include "../platform/win32/Win32Backend.hpp"

#include <utility>

namespace WebGUI {

Window::Window(const char* title, int width, int height)
    : m_impl(std::make_unique<Impl>(title, width, height)) {}

Window::~Window() = default;
Window::Window(Window&&) noexcept = default;
Window& Window::operator=(Window&&) noexcept = default;

bool Window::valid() const { return m_impl && m_impl->valid(); }
bool Window::load(const std::string& file) { return m_impl && m_impl->load(file); }
void Window::show() { if (m_impl) m_impl->show(); }
bool Window::poll_events() { return m_impl && m_impl->poll_events(); }
void Window::close() { if (m_impl) m_impl->close(); }
void Window::post_message(const std::string& message) { if (m_impl) m_impl->post_message(message); }
void Window::execute_script(const std::string& script) { if (m_impl) m_impl->execute_script(script); }
void Window::on_message(MessageHandler handler) { if (m_impl) m_impl->on_message(std::move(handler)); }

void Window::run() {
    while (poll_events()) {
        WaitMessage();
    }
}

Window::Impl::Impl(const char* title, int width, int height)
    : m_backend(std::make_unique<Win32Backend>()) {
    m_backend->Init(title, width, height, [this](const std::string& message) {
        if (m_handler) m_handler(message);
    });
}

bool Window::Impl::valid() const { return m_backend && m_backend->Valid(); }
bool Window::Impl::load(const std::string& file) { return m_backend && m_backend->Load(file); }
void Window::Impl::show() { if (m_backend) m_backend->Show(); }
bool Window::Impl::poll_events() { return m_backend && m_backend->PumpEvents(); }
void Window::Impl::close() { if (m_backend) m_backend->Close(); }
void Window::Impl::post_message(const std::string& message) { if (m_backend) m_backend->PostMessage(message); }
void Window::Impl::execute_script(const std::string& script) { if (m_backend) m_backend->ExecuteScript(script); }
void Window::Impl::on_message(MessageHandler handler) { m_handler = std::move(handler); }

} // namespace WebGUI
