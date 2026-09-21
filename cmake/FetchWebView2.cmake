include(FetchContent)

set(WEBVIEW2_VERSION "1.0.2478.35")

FetchContent_Declare(webview2
    URL "https://globalcdn.nuget.org/packages/microsoft.web.webview2.${WEBVIEW2_VERSION}.nupkg"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(webview2)

set(WEBVIEW2_INCLUDE_DIR "${webview2_SOURCE_DIR}/build/native/include"             CACHE INTERNAL "")
set(WEBVIEW2_LOADER_LIB  "${webview2_SOURCE_DIR}/build/native/x64/WebView2Loader.dll.lib" CACHE INTERNAL "")
set(WEBVIEW2_LOADER_DLL  "${webview2_SOURCE_DIR}/build/native/x64/WebView2Loader.dll"     CACHE INTERNAL "")

if(NOT EXISTS "${WEBVIEW2_INCLUDE_DIR}/WebView2.h")
    message(FATAL_ERROR "WebView2.h not found after download. Check that the nupkg URL is correct for version ${WEBVIEW2_VERSION}.")
endif()
