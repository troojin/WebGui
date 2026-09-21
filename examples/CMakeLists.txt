add_subdirectory(basic)
add_subdirectory(widgets)
add_subdirectory(dll)

set_property(TARGET basic widgets pure_webgui_plugin dll_host PROPERTY FOLDER "Examples")
