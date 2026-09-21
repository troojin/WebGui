# Pure WebGUI

Pure WebGUI is a small immediate-mode C++ GUI library for Windows. You write familiar C++ widget calls; the library renders them with an embedded WebView2 browser. It is useful for lightweight native tools when you want a simple C++ API and web-based presentation.

## What it can do

- Create a native Windows window backed by WebView2.
- Display text, buttons, checkboxes, text inputs, and separators.
- Send button clicks and value changes back to C++.
- Keep each frame declarative: describe the current UI, then call `Render()`.

## Requirements

- Windows 10 or 11 (x64)
- [WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) (included with Windows 11 and current Microsoft Edge installations)
- CMake 3.24 or newer
- Visual Studio 2019 or newer with MSVC and C++17 support
- Internet access on the first CMake configure, to download the WebView2 SDK and nlohmann/json

## Build and run

From a Developer Command Prompt for Visual Studio:

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The build creates an `Examples` workspace folder in the generated Visual Studio solution with three demos:

```powershell
build\bin\Release\basic.exe
build\bin\Release\widgets.exe
build\bin\Release\dll_host.exe
```

`pure_webgui_copy_assets()` copies `web/` and `WebView2Loader.dll` beside each demo. If you create your own executable, call it after linking `PureWebGUI`:

```cmake
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE PureWebGUI)
pure_webgui_copy_assets(my_app)
```

## Using Pure WebGUI from a DLL

Pure WebGUI can be linked into a DLL. Export a normal function from the DLL and have the host call it after `LoadLibrary`; do **not** create a window or call Pure WebGUI from `DllMain`. The GUI runs its message loop on the thread that calls `pure::Create`, so the host should use a dedicated thread if it must continue doing other work.

The `examples/dll` project provides a complete `pure_webgui_plugin.dll` and `dll_host.exe` pair. The host loads the DLL, resolves its exported `RunPureWebGUIDemo` function, and runs it on a worker thread. The `web/` directory and `WebView2Loader.dll` must be copied beside the **host executable**, since the backend resolves assets from the process executable directory:

```cmake
add_library(my_gui_plugin SHARED plugin.cpp)
target_link_libraries(my_gui_plugin PRIVATE PureWebGUI)

add_executable(my_host host.cpp)
pure_webgui_copy_assets(my_host)
```

The DLL and host executable also need to be placed where the host can load the DLL, typically in the same output directory as shown by the included example.

## Quick start

```cpp
#include <PureWebGUI/PureWebGUI.hpp>
#include <string>

int main() {
    if (!pure::Create("My App", 1024, 768)) return 1;

    int clicks = 0;
    while (pure::Running()) {
        pure::Begin("My App");
        pure::Text("Hello, Pure WebGUI!");
        if (pure::Button("Click me")) ++clicks;
        pure::Text(("Clicks: " + std::to_string(clicks)).c_str());
        pure::End();
        pure::Render();
    }

    pure::Shutdown();
}
```

The `examples/basic` program contains a complete version.

## How the frame loop works

Call these functions in this order on every iteration:

1. `Running()` reports whether the native window is still open.
2. `Begin()` starts a new frame and clears the previous frame's widgets.
3. Add widgets in the order that you want them displayed.
4. `End()` finishes the widget list.
5. `Render()` sends that list to WebView2 and processes Windows events.
6. Call `Shutdown()` once after the loop.

Widgets return changes that happened since the previous frame. This means a button press or input edit is handled by C++ on the next trip through the loop, just like most immediate-mode GUIs.

## Widget reference

| Function | Description |
| --- | --- |
| `Text(text)` | Shows a line of text. |
| `Button(label)` | Shows a button and returns `true` once when it is clicked. |
| `Checkbox(label, &value)` | Shows a checkbox, updates `value`, and returns `true` when it changes. |
| `InputText(label, &value)` | Shows a text field, updates `value`, and returns `true` when its text changes. |
| `Separator()` | Shows a horizontal divider. |

`Checkbox` and `InputText` require a valid pointer. Passing `nullptr` does nothing and returns `false`.

### Labels and IDs

By default, each widget's label is also its event ID, so labels must be unique within a frame. Where a duplicate visible label is useful, use the overload with an explicit unique ID:

```cpp
pure::Button("Delete", "delete-draft");
pure::Button("Delete", "delete-published");
```

`Button`, `Checkbox`, and `InputText` all have this overload.

## Examples

### Settings form

```cpp
bool notifications = true;
std::string display_name = "Ada";

while (pure::Running()) {
    pure::Begin("Settings");
    pure::InputText("Display name", &display_name);
    pure::Checkbox("Enable notifications", &notifications);
    pure::Separator();

    if (pure::Button("Save", "settings-save")) {
        SaveSettings(display_name, notifications);
    }
    pure::End();
    pure::Render();
}
```

### Repeated action buttons

Use explicit IDs when rendering repeated labels:

```cpp
for (int i = 0; i < 3; ++i) {
    const std::string id = "remove-" + std::to_string(i);
    if (pure::Button("Remove", id.c_str())) {
        RemoveItem(i);
    }
}
```

### DLL entry point

Keep the Pure WebGUI lifecycle outside `DllMain` and expose a callable function instead:

```cpp
extern "C" __declspec(dllexport) int RunGui() {
    if (!pure::Create("Plugin UI")) return 1;

    while (pure::Running()) {
        pure::Begin("Plugin UI");
        pure::Text("Running from a DLL");
        pure::End();
        pure::Render();
    }

    pure::Shutdown();
    return 0;
}
```

## Architecture

```
C++ API  ->  Command buffer  ->  JSON  ->  WebView2  ->  JS / HTML / CSS
                                                       |
                                          C++ <- JSON <- user events
```

Each `Render()` serializes the current command list and sends it to the embedded page. JavaScript renders the DOM and posts clicks or changed field values back to C++. Those events are consumed by the corresponding widget during a later frame.

## Current limits

- Windows x64 and WebView2 are currently required.
- The library is intentionally small; it does not yet provide layouts, menus, images, or persistence.
- Rendering replaces the page's widget DOM each frame, so it is best suited to simple tools and prototypes.
