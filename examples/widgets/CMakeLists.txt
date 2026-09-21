add_executable(widgets main.cpp)

target_link_libraries(widgets PRIVATE PureWebGUI)

pure_webgui_copy_assets(widgets)
