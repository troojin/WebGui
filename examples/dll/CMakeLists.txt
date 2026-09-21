add_library(pure_webgui_plugin SHARED plugin.cpp)
target_link_libraries(pure_webgui_plugin PRIVATE PureWebGUI)

add_executable(dll_host host.cpp)
add_dependencies(dll_host pure_webgui_plugin)

# The backend loads web assets beside the process executable, not beside the DLL.
pure_webgui_copy_assets(dll_host)
