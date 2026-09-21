add_executable(basic main.cpp)
target_link_libraries(basic PRIVATE WebGUI::WebGUI)
webgui_copy_runtime(basic)

add_custom_command(TARGET basic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/web"
        "$<TARGET_FILE_DIR:basic>/web"
)
set_property(TARGET basic PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:basic>")
