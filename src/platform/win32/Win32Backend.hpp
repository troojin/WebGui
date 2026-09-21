#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <wrl.h>
#include <wrl/event.h>
#include <WebView2.h>
#include <functional>
#include <string>

class Win32Backend {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    Win32Backend() = default;
    ~Win32Backend();

    Win32Backend(const Win32Backend&) = delete;
    Win32Backend& operator=(const Win32Backend&) = delete;

    bool Init(const char* title, int w, int h, MessageCallback on_message);
    bool Running() const { return m_running; }
    void PostMessage(const std::string& json);
    void PumpEvents();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    HWND m_hwnd = nullptr;
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_env;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller>  m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2>            m_webview;
    EventRegistrationToken m_msg_token  = {};
    MessageCallback        m_on_message;
    bool                   m_running    = false;
};
