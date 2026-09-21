add_executable(embedded WIN32 main.cpp)
target_link_libraries(embedded PRIVATE WebGUI::WebGUI)
webgui_copy_runtime(embedded)

add_custom_command(TARGET embedded POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/web"
        "$<TARGET_FILE_DIR:embedded>/web"
)
set_property(TARGET embedded PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:embedded>")
