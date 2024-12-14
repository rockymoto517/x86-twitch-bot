cmake_minimum_required(VERSION 3.15)

# vcpkg setup
set(VCPKG_TARGET_ARCHITECTURE x86)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_TARGET_TRIPLET x86-windows-static)

project(TwitchBot VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE release)
# For convenience's sake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform setup
add_compile_definitions(WIN32)
add_compile_definitions(_WIN32)

file(GLOB_RECURSE sources src/*.cpp)
add_executable(twitch_bot ${sources})
target_include_directories(twitch_bot PUBLIC include)
set_property(TARGET twitch_bot PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# cURL
option(CURL_STATICLIB ON)
add_definitions("-DCURL_STATICLIB")
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
target_link_libraries(twitch_bot PRIVATE CURL::libcurl)

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(twitch_bot PRIVATE OpenSSL::SSL)
target_link_libraries(twitch_bot PRIVATE OpenSSL::Crypto)

# boost-random
find_package(boost_random REQUIRED)
target_link_libraries(twitch_bot PRIVATE Boost::random)

# websocketpp
find_package(websocketpp REQUIRED)
target_link_libraries(twitch_bot PRIVATE websocketpp::websocketpp)

# fmt
find_package(fmt REQUIRED)
target_link_libraries(twitch_bot PRIVATE fmt::fmt)

# nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(twitch_bot PRIVATE nlohmann_json::nlohmann_json)
