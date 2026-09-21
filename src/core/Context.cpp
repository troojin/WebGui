#include "Context.hpp"
#include "../platform/win32/Win32Backend.hpp"
#include <PureWebGUI/PureWebGUI.hpp>
#include <nlohmann/json.hpp>

#include <type_traits>

namespace pure {

static Context* g_ctx = nullptr;

bool Create(const char* title, int width, int height) {
    g_ctx = new Context();
    if (!g_ctx->Init(title, width, height)) {
        delete g_ctx;
        g_ctx = nullptr;
        return false;
    }
    return true;
}

bool Running()                { return g_ctx && g_ctx->Running(); }
void Begin(const char* title) { if (g_ctx) g_ctx->Begin(title); }
void End()                    { if (g_ctx) g_ctx->End(); }
void Text(const char* text)   { if (g_ctx) g_ctx->Text(text); }
bool Button(const char* label){ return g_ctx ? g_ctx->Button(label) : false; }
bool Button(const char* label, const char* id) { return g_ctx ? g_ctx->Button(label, id) : false; }
bool Checkbox(const char* label, bool* value) { return g_ctx ? g_ctx->Checkbox(label, value) : false; }
bool Checkbox(const char* label, bool* value, const char* id) { return g_ctx ? g_ctx->Checkbox(label, value, id) : false; }
bool InputText(const char* label, std::string* value) { return g_ctx ? g_ctx->InputText(label, value) : false; }
bool InputText(const char* label, std::string* value, const char* id) { return g_ctx ? g_ctx->InputText(label, value, id) : false; }
void Separator() { if (g_ctx) g_ctx->Separator(); }
void Render()                 { if (g_ctx) g_ctx->Render(); }

void Shutdown() {
    delete g_ctx;
    g_ctx = nullptr;
}

} // namespace pure

Context::Context() = default;
Context::~Context() = default;

bool Context::Init(const char* title, int w, int h) {
    backend = std::make_unique<Win32Backend>();
    return backend->Init(title, w, h, [this](const std::string& raw) {
        try {
            auto j = nlohmann::json::parse(raw);
            if (j.value("type", std::string{}) == "click") {
                auto id = j.value("id", std::string{});
                if (!id.empty()) pending_clicks.insert(std::move(id));
            } else if (j.value("type", std::string{}) == "checkbox") {
                auto id = j.value("id", std::string{});
                if (!id.empty()) pending_checks[id] = j.value("value", false);
            } else if (j.value("type", std::string{}) == "input") {
                auto id = j.value("id", std::string{});
                if (!id.empty()) pending_text[id] = j.value("value", std::string{});
            }
        } catch (...) {}
    });
}

bool Context::Running() const {
    return backend && backend->Running();
}

void Context::Begin(const char* title) {
    commands.clear();
    commands.push_back(CmdBegin{title});
}

void Context::End() {
    commands.push_back(CmdEnd{});
}

void Context::Text(const char* text) {
    commands.push_back(CmdText{text});
}

bool Context::Button(const char* label) {
    return Button(label, label);
}

bool Context::Button(const char* label, const char* id) {
    commands.push_back(CmdButton{id, label});
    return ConsumeClick(id);
}

bool Context::Checkbox(const char* label, bool* value) {
    return Checkbox(label, value, label);
}

bool Context::Checkbox(const char* label, bool* value, const char* id) {
    if (!value) return false;
    const bool changed = ConsumeCheckbox(id, value);
    commands.push_back(CmdCheckbox{id, label, *value});
    return changed;
}

bool Context::InputText(const char* label, std::string* value) {
    return InputText(label, value, label);
}

bool Context::InputText(const char* label, std::string* value, const char* id) {
    if (!value) return false;
    const bool changed = ConsumeText(id, value);
    commands.push_back(CmdInputText{id, label, *value});
    return changed;
}

void Context::Separator() {
    commands.push_back(CmdSeparator{});
}

bool Context::ConsumeClick(const std::string& id) {
    auto it = pending_clicks.find(id);
    if (it == pending_clicks.end()) return false;
    pending_clicks.erase(it);
    return true;
}

bool Context::ConsumeCheckbox(const std::string& id, bool* value) {
    auto it = pending_checks.find(id);
    if (it == pending_checks.end()) return false;
    *value = it->second;
    pending_checks.erase(it);
    return true;
}

bool Context::ConsumeText(const std::string& id, std::string* value) {
    auto it = pending_text.find(id);
    if (it == pending_text.end()) return false;
    *value = std::move(it->second);
    pending_text.erase(it);
    return true;
}

void Context::Render() {
    using json = nlohmann::json;
    json arr = json::array();

    for (const auto& cmd : commands) {
        std::visit([&arr](const auto& c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, CmdBegin>) {
                arr.push_back({{"type", "begin"}, {"title", c.title}});
            } else if constexpr (std::is_same_v<T, CmdEnd>) {
                arr.push_back({{"type", "end"}});
            } else if constexpr (std::is_same_v<T, CmdText>) {
                arr.push_back({{"type", "text"}, {"text", c.text}});
            } else if constexpr (std::is_same_v<T, CmdButton>) {
                arr.push_back({{"type", "button"}, {"id", c.id}, {"label", c.label}});
            } else if constexpr (std::is_same_v<T, CmdCheckbox>) {
                arr.push_back({{"type", "checkbox"}, {"id", c.id}, {"label", c.label}, {"checked", c.checked}});
            } else if constexpr (std::is_same_v<T, CmdInputText>) {
                arr.push_back({{"type", "input"}, {"id", c.id}, {"label", c.label}, {"value", c.value}});
            } else if constexpr (std::is_same_v<T, CmdSeparator>) {
                arr.push_back({{"type", "separator"}});
            }
        }, cmd);
    }

    backend->PostMessage(arr.dump());
    backend->PumpEvents();
}
