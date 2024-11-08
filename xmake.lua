add_rules("mode.debug", "mode.release")
set_project("twitch_bot")

-- External libs
local libs = {
	"libcurl",
	"openssl",
	"vcpkg::boost-random",
	"vcpkg::websocketpp",
	"fmt",
	"date",
	"nlohmann_json",
}
add_requires(table.unpack(libs))

target("twitch_bot")
set_kind("binary")
set_arch("x86")

set_languages("c++20")

add_files("src/*.cpp")

add_packages(table.unpack(libs))
target_end()
