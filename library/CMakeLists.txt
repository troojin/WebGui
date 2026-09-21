cmake_minimum_required(VERSION 3.24)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(WebGUI VERSION 0.1.0 LANGUAGES CXX)
endif()

if(NOT WIN32)
    message(FATAL_ERROR "WebGUI currently supports Windows only.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "WebGUI requires a 64-bit (x64) build.")
endif()

include(FetchContent)

set(WEBGUI_WEBVIEW2_VERSION "1.0.2478.35")
FetchContent_Declare(webview2
    URL "https://globalcdn.nuget.org/packages/microsoft.web.webview2.${WEBGUI_WEBVIEW2_VERSION}.nupkg"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(webview2)

set(WEBGUI_WEBVIEW2_ROOT "${webview2_SOURCE_DIR}/build/native")
if(NOT EXISTS "${WEBGUI_WEBVIEW2_ROOT}/include/WebView2.h")
    message(FATAL_ERROR "WebView2 SDK download did not contain WebView2.h.")
endif()

add_library(WebGUI STATIC
    src/core/Context.cpp
    src/platform/win32/Win32Backend.cpp
)
add_library(WebGUI::WebGUI ALIAS WebGUI)

target_compile_features(WebGUI PUBLIC cxx_std_17)
target_include_directories(WebGUI
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${WEBGUI_WEBVIEW2_ROOT}/include
)
target_link_libraries(WebGUI PRIVATE
    "${WEBGUI_WEBVIEW2_ROOT}/x64/WebView2Loader.dll.lib"
    ole32
    oleaut32
)
target_compile_definitions(WebGUI PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)

# Copies the WebView2 loader next to an executable. Call after add_executable.
function(webgui_copy_runtime target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${WEBGUI_WEBVIEW2_ROOT}/x64/WebView2Loader.dll"
            "$<TARGET_FILE_DIR:${target}>"
        VERBATIM
    )
endfunction()
