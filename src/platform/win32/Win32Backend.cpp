#include "Win32Backend.hpp"

using namespace Microsoft::WRL;

static std::wstring to_wide(const std::string& s) {
    if (s.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring result(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), result.data(), len);
    return result;
}

static std::string to_narrow(const wchar_t* ws) {
    if (!ws || ws[0] == L'\0') return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, ws, -1, nullptr, 0, nullptr, nullptr);
    if (len <= 1) return {};
    std::string result(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws, -1, result.data(), len, nullptr, nullptr);
    result.pop_back();
    return result;
}

static std::wstring ExeDir() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring p(path);
    auto sep = p.find_last_of(L"\\/");
    return (sep != std::wstring::npos) ? p.substr(0, sep) : p;
}

LRESULT CALLBACK Win32Backend::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Win32Backend* self = nullptr;
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<Win32Backend*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<Win32Backend*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    switch (msg) {
    case WM_SIZE:
        if (self && self->m_controller) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            self->m_controller->put_Bounds(rc);
        }
        return 0;

    case WM_DESTROY:
        if (self) self->m_hwnd = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

bool Win32Backend::Init(const char* title, int w, int h, MessageCallback on_message) {
    m_on_message = std::move(on_message);

    WNDCLASSEXW wc   = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"PureWebGUI";
    RegisterClassExW(&wc);

    m_hwnd = CreateWindowExW(
        0, L"PureWebGUI", to_wide(title).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,
        nullptr, nullptr, GetModuleHandleW(nullptr),
        this
    );
    if (!m_hwnd) return false;

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    std::wstring web_root = ExeDir() + L"\\web";

    bool    env_done = false;
    HRESULT env_hr   = S_OK;

    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                env_hr = result;
                if (SUCCEEDED(result)) m_env = env;
                env_done = true;
                return S_OK;
            }
        ).Get()
    );
    if (FAILED(hr)) return false;

    {
        MSG pump;
        while (!env_done) {
            int r = GetMessage(&pump, nullptr, 0, 0);
            if (r == 0) { PostQuitMessage(static_cast<int>(pump.wParam)); return false; }
            if (r < 0)  return false;
            TranslateMessage(&pump);
            DispatchMessage(&pump);
        }
    }

    if (FAILED(env_hr) || !m_env) return false;

    bool    ctrl_done = false;
    HRESULT ctrl_hr   = S_OK;

    hr = m_env->CreateCoreWebView2Controller(
        m_hwnd,
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [&](HRESULT result, ICoreWebView2Controller* ctrl) -> HRESULT {
                ctrl_hr = result;
                if (SUCCEEDED(result)) {
                    m_controller = ctrl;
                    m_controller->get_CoreWebView2(&m_webview);
                }
                ctrl_done = true;
                return S_OK;
            }
        ).Get()
    );
    if (FAILED(hr)) return false;

    {
        MSG pump;
        while (!ctrl_done) {
            int r = GetMessage(&pump, nullptr, 0, 0);
            if (r == 0) { PostQuitMessage(static_cast<int>(pump.wParam)); return false; }
            if (r < 0)  return false;
            TranslateMessage(&pump);
            DispatchMessage(&pump);
        }
    }

    if (FAILED(ctrl_hr) || !m_controller || !m_webview) return false;

    ComPtr<ICoreWebView2Settings> settings;
    m_webview->get_Settings(&settings);
    if (settings) {
        settings->put_IsWebMessageEnabled(TRUE);
        settings->put_AreDevToolsEnabled(TRUE);
        settings->put_IsStatusBarEnabled(FALSE);
    }

    RECT rc;
    GetClientRect(m_hwnd, &rc);
    m_controller->put_Bounds(rc);

    // serve web/ as https://pure.local/ so that relative paths in HTML/CSS/JS resolve correctly.
    ComPtr<ICoreWebView2_3> webview3;
    if (FAILED(m_webview.As(&webview3))) return false;

    webview3->SetVirtualHostNameToFolderMapping(
        L"pure.local",
        web_root.c_str(),
        COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW
    );

    m_webview->add_WebMessageReceived(
        Callback<ICoreWebView2WebMessageReceivedEventHandler>(
            [this](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                wchar_t* raw = nullptr;
                if (SUCCEEDED(args->TryGetWebMessageAsString(&raw)) && raw) {
                    m_on_message(to_narrow(raw));
                    CoTaskMemFree(raw);
                }
                return S_OK;
            }
        ).Get(),
        &m_msg_token
    );

    bool nav_done = false;
    EventRegistrationToken nav_token;
    m_webview->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [&](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs*) -> HRESULT {
                nav_done = true;
                return S_OK;
            }
        ).Get(),
        &nav_token
    );

    m_webview->Navigate(L"https://pure.local/index.html");

    {
        MSG pump;
        while (!nav_done) {
            int r = GetMessage(&pump, nullptr, 0, 0);
            if (r == 0) { PostQuitMessage(static_cast<int>(pump.wParam)); return false; }
            if (r < 0)  return false;
            TranslateMessage(&pump);
            DispatchMessage(&pump);
        }
    }

    m_webview->remove_NavigationCompleted(nav_token);

    m_running = true;
    return true;
}

Win32Backend::~Win32Backend() {
    if (m_webview) m_webview->remove_WebMessageReceived(m_msg_token);
    m_webview.Reset();
    m_controller.Reset();
    m_env.Reset();

    if (m_hwnd && IsWindow(m_hwnd)) {
        DestroyWindow(m_hwnd);
        MSG pump;
        while (PeekMessage(&pump, nullptr, 0, 0, PM_REMOVE)) {
            if (pump.message == WM_QUIT) break;
            TranslateMessage(&pump);
            DispatchMessage(&pump);
        }
    }
}

void Win32Backend::PostMessage(const std::string& json) {
    if (m_webview) m_webview->PostWebMessageAsString(to_wide(json).c_str());
}

void Win32Backend::PumpEvents() {
    MSG pump;
    while (PeekMessage(&pump, nullptr, 0, 0, PM_REMOVE)) {
        if (pump.message == WM_QUIT) {
            m_running = false;
            return;
        }
        TranslateMessage(&pump);
        DispatchMessage(&pump);
    }
}
