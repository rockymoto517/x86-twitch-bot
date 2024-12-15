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
static std::string channel_name;
static std::string bot_client_id;
static std::string secret;
static std::string refresh_token;
static std::string channel_id;

void main_loop() {
    bot = new w_twitch(channel_name, bot_client_id);
    while (!loop_switch) {
        if (!Bot::do_loop(bot, channel_id, bot_client_id, secret)) {
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
    PD2HOOK_LOG_LOG("Attempted to start loop.");
    return 0;
}

int end_loop_thread(lua_State *L) {
    loop_switch = true;
    return 0;
}

int get_messages(lua_State *L) {
    auto [usr, msg] = bot->get_message();
    lua_pushstring(L, usr.c_str());
    lua_pushstring(L, msg.c_str());

    return 2;
}

int set_auth_info(lua_State *L) {
    bot_client_id = lua_tostring(L, 1);
    secret = lua_tostring(L, 2);
    refresh_token = lua_tostring(L, 3);
    channel_id = lua_tostring(L, 4);
    channel_name = lua_tostring(L, 5);
    return 0;
}

int setup(lua_State *L) {
    try {
        Curl::refresh_token(bot_client_id, secret, refresh_token);
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
        PD2HOOK_LOG_LOG("Auth token completed.");

    } catch (std::exception &e) {
        PD2HOOK_LOG_LOG(
            fmt::format("Exception in main: {}\n", e.what()).c_str());
    }
    return 0;
}

void Plugin_Init() {}

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

    lua_pushcfunction(L, set_auth_info);
    lua_setfield(L, -2, "set_auth_info");

    lua_pushcfunction(L, setup);
    lua_setfield(L, -2, "setup");

    return 1;
}
