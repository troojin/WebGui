cmake_minimum_required(VERSION 3.24)
project(PureWebGUI VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "PureWebGUI currently requires a 64-bit (x64) build.")
endif()

include(cmake/FetchWebView2.cmake)
include(cmake/FetchJSON.cmake)

add_library(PureWebGUI STATIC
    src/core/Context.cpp
    src/platform/win32/Win32Backend.cpp
)

target_include_directories(PureWebGUI
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${WEBVIEW2_INCLUDE_DIR}
)

target_link_libraries(PureWebGUI
    PRIVATE
        nlohmann_json::nlohmann_json
        ${WEBVIEW2_LOADER_LIB}
        shlwapi
        ole32
        oleaut32
)

target_compile_definitions(PureWebGUI PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

set(PUREWEBGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")

# Copies WebView2Loader.dll and the web/ directory next to the target executable.
# Call this on every executable that links PureWebGUI.
function(pure_webgui_copy_assets target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${WEBVIEW2_LOADER_DLL}"
            "$<TARGET_FILE_DIR:${target}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PUREWEBGUI_SOURCE_DIR}/web"
            "$<TARGET_FILE_DIR:${target}>/web"
        VERBATIM
    )
endfunction()

add_subdirectory(examples)
