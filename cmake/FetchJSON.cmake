include(FetchContent)

set(JSON_BuildTests          OFF CACHE INTERNAL "")
set(JSON_Install             OFF CACHE INTERNAL "")
set(JSON_ImplicitConversions OFF CACHE INTERNAL "")

FetchContent_Declare(nlohmann_json
    URL "https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)
