#include "botFuncs.hpp"

#include <date/date.h>
#include <fmt/printf.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "curlWrapper.hpp"

namespace Bot {
using json = nlohmann::json;
void init_session(w_twitch *bot, const json &res) {
    if (!res.contains("payload")) throw("Error finding initializer payload.");
    if (!res["payload"].contains("session"))
        throw("Error finding initializer payload.");

    if (!res["payload"]["session"].contains("id"))
        throw("Error finding finding session id in initializer payload.");
    bot->session_id =
        res["payload"]["session"]["id"].template get<std::string>();

    if (!res["payload"]["session"].contains("keepalive_timeout_seconds"))
        throw("Error finding keepalive timer in initializer payload.");
    bot->keepalive_timeout =
        res["payload"]["session"]["keepalive_timeout_seconds"]
            .template get<uint32_t>();
    std::string res2 = Curl::get_token().value();
    std::string res3 = Curl::subscribe(bot->session_id).value();
    fmt::print("{}\n", res3);
}

uint32_t parse_8601(std::istringstream &&is) {
    std::string fallback;
    is >> fallback;
    std::istringstream in{fallback};
    date::sys_time<std::chrono::seconds> tp;
    in >> date::parse("%FT%TZ", tp);
    if (in.fail()) {
        in.clear();
        in.exceptions(std::ios::failbit);
        in.str(fallback);
        in >> date::parse("%FT%T%Ez", tp);
    }
    return static_cast<uint32_t>(tp.time_since_epoch().count());
}

// Eventually update this to check even if no keepalive message was sent
bool check_keep_alive(const json &res, w_twitch *bot) {
    if (!res["message_timestamp"])
        throw("Error: Keepalive message did not contain a timestamp.");
    uint32_t timestamp = parse_8601(std::istringstream{
        res["message_timestamp"].template get<std::string>()});
    return bot->update_keepalive(timestamp);
}

bool do_loop(w_twitch *bot) {
    std::string msg = bot->get_msg();
    if (msg == "") return true;

    json res = json::parse(msg);
    if (res.contains("metadata")) {
        if (res["metadata"].contains("message_type")) {
            if (res["metadata"]["message_type"].template get<std::string>() ==
                "session_welcome") {
                init_session(bot, res);
                return true;
            }
            if (res["metadata"]["message_type"].template get<std::string>() ==
                "session_keepalive") {
                return check_keep_alive(res["metadata"], bot);
            }
        }
    }
}
};  // namespace Bot
