#include "botFuncs.hpp"

#include <fmt/printf.h>

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "curlWrapper.hpp"

namespace Bot {
using json = nlohmann::json;

// Don't nest the ifs so the code is actually readable...
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
    std::string _ = Curl::get_token().value();
    _ = Curl::subscribe(bot->session_id).value();
    fmt::print(
        "Session token acquired and scopes subscribed. Receiving messages "
        "now.\n");
}

// Eventually update this to check even if no keepalive message was sent
bool check_keep_alive(const json &res, w_twitch *bot) {
    auto now = std::chrono::system_clock::now();
    int timestamp = static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
            .count());
    return bot->update_keepalive(timestamp);
}

// Grab the message from the EventSub notification
void handle_notification(const json &res, w_twitch *bot) {
    if (!res.contains("event")) {
        fmt::print("Error: No payload event found.\n");
        return;
    }

    if (!res["event"].contains("message")) {
        fmt::print("Error: Payload contains no message.\n");
        return;
    }

    if (!res["event"]["message"].contains("text")) {
        fmt::print("Error: Payload message contains no test.\n");
        return;
    }

    std::string msg =
        res["event"]["message"]["text"].template get<std::string>();
    std::string usr = "username_not_found";
    if (res["event"].contains("chatter_user_name")) {
        usr = res["event"]["chatter_user_name"].template get<std::string>();
    }

    bot->store_message(msg, usr);
}

// Meat and potatoes of the boat, handles every message
// Returns false if the bot disconnects
// Returns true else
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
            if (res["metadata"]["message_type"].template get<std::string>() ==
                "notification") {
                if (res.contains("payload")) {
                    handle_notification(res["payload"], bot);
                } else {
                    fmt::print("Error: no notification payload.\n");
                }
                return true;
            }
        }
    }
    return true;
}
};  // namespace Bot
