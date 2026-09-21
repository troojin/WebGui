#pragma once
#include <string>
#include <variant>

struct CmdBegin  { std::string title; };
struct CmdEnd    {};
struct CmdText   { std::string text; };
struct CmdButton { std::string id; std::string label; };
struct CmdCheckbox { std::string id; std::string label; bool checked; };
struct CmdInputText { std::string id; std::string label; std::string value; };
struct CmdSeparator {};

using Command = std::variant<CmdBegin, CmdEnd, CmdText, CmdButton, CmdCheckbox, CmdInputText, CmdSeparator>;
