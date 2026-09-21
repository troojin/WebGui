#pragma once
#include "Commands.hpp"
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

class Win32Backend;

struct Context {
    std::vector<Command>            commands;
    std::unordered_set<std::string> pending_clicks;
    std::unordered_map<std::string, bool> pending_checks;
    std::unordered_map<std::string, std::string> pending_text;
    std::unique_ptr<Win32Backend>   backend;

    Context();
    ~Context();

    bool Init(const char* title, int w, int h);
    bool Running() const;
    void Begin(const char* title);
    void End();
    void Text(const char* text);
    bool Button(const char* label);
    bool Button(const char* label, const char* id);
    bool Checkbox(const char* label, bool* value);
    bool Checkbox(const char* label, bool* value, const char* id);
    bool InputText(const char* label, std::string* value);
    bool InputText(const char* label, std::string* value, const char* id);
    void Separator();
    void Render();

private:
    bool ConsumeClick(const std::string& id);
    bool ConsumeCheckbox(const std::string& id, bool* value);
    bool ConsumeText(const std::string& id, std::string* value);
};
