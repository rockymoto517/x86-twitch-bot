add_rules("mode.debug", "mode.release")
set_project("twitch_bot")

-- External libs
local libs = {
	"libcurl",
	"openssl",
	"vcpkg::boost-random", -- Not available in the xmake package manager
	"vcpkg::websocketpp", -- Also not available in xrepo...
	"fmt",
	"nlohmann_json",
}
add_requires(table.unpack(libs))

target("twitch_bot")
set_kind("binary")
set_arch("x86")

set_languages("c++17")

add_files("src/*.cpp")
add_includedirs("include")

add_packages(table.unpack(libs))
target_end()
