#pragma once

#include <string>

namespace pure {

bool Create(const char* title = "Pure WebGUI", int width = 1024, int height = 768);
bool Running();

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
void Shutdown();

} // namespace pure
