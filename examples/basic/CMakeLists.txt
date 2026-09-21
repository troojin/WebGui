add_executable(basic main.cpp)

target_link_libraries(basic PRIVATE PureWebGUI)

pure_webgui_copy_assets(basic)
