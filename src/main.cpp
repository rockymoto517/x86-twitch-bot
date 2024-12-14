#include <fmt/printf.h>
#include <superblt_flat.h>

#include <nlohmann/json.hpp>
#include <thread>

#include "botFuncs.hpp"
#include "constants.hpp"
#include "curlWrapper.hpp"
#include "superblt_flat.h"

using json = nlohmann::json;
static w_twitch *bot = nullptr;
static bool loop_switch = false;
static std::thread loop;

void main_loop() {
    bot = new w_twitch("rockytestdll3");
    while (!loop_switch) {
        if (!Bot::do_loop(bot)) {
            PD2HOOK_LOG_LOG(fmt::format("Disconnected.\n").c_str());
            delete bot;
            break;
        }
    }
    loop_switch = false;
    loop.join();
}

int start_loop_async(lua_State *L) {
    loop = std::thread(main_loop);
    return 0;
}

int end_loop_thread(lua_State *L) {
    loop_switch = true;
    return 0;
}

int get_messages(lua_State *L) {
    auto [usr, msg] = bot->get_message();
    if (usr == "") {
        lua_pushnil(L);
        return 1;
    }

    lua_newtable(L);
    uint8_t index = 1;
    while (usr != "") {
        lua_pushinteger(L, index);
        lua_newtable(L);
        lua_pushstring(L, usr.c_str());
        lua_pushstring(L, msg.c_str());
        lua_settable(L, -3);
        lua_settable(L, -3);

        index++;
        std::tie(usr, msg) = bot->get_message();
    }
    return 1;
}

void Plugin_Init() {
    try {
        Curl::refresh_token();
        std::optional<std::string> res = Curl::get_auth(AUTH_URL);
        if (res) {
            json resJson = json::parse(res.value());
            if (resJson.contains("status")) {
                uint16_t status = resJson["status"].template get<uint16_t>();
                PD2HOOK_LOG_LOG(
                    fmt::format("Status code: {}\n", status).c_str());
            }
        }
        // std::thread loop_thread(main_loop);

    } catch (std::exception &e) {
        PD2HOOK_LOG_LOG(
            fmt::format("Exception in main: {}\n", e.what()).c_str());
    }
}

void Plugin_Update() {}

void Plugin_Setup_Lua(lua_State *L) {}

int Plugin_PushLua(lua_State *L) {
    lua_newtable(L);

    lua_pushcfunction(L, start_loop_async);
    lua_setfield(L, -2, "listen");

    lua_pushcfunction(L, end_loop_thread);
    lua_setfield(L, -2, "destroy");

    lua_pushcfunction(L, get_messages);
    lua_setfield(L, -2, "get_messages");

    return 1;
}
