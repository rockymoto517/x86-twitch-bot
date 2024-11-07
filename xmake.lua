add_rules("mode.debug", "mode.release")
set_project("twitch_bot")

-- External libs
add_requires("libcurl", "cpr", "asio", "fmt", "nlohmann_json")

target("twitch_bot")
set_kind("binary")
set_arch("x86")

set_languages("c++17")

add_files("src/*.cpp")

add_packages("libcurl", "cpr", "asio", "fmt", "nlohmann_json")
target_end()
