#include "clientWrapper.hpp"

#include <fmt/printf.h>

#include <cstdint>

#include "constants.hpp"
#include "curlWrapper.hpp"

w_twitch::w_twitch(const std::string &name) : channel_name(name) {
    Curl::login(name);
    id = endpoint.connect(WEBSOCKET_URL);
    if (id == -1) {
        // PD2HOOK_LOG_LOG("Error opening socket.");
        fmt::print("Error opening socket.\n");
        return;
    }

    while (true) {
        connection_metadata::ptr meta = endpoint.get_metadata(id);
        if (meta->get_status() == "Open") {
            break;
        }
    }
}

w_twitch::~w_twitch() {}

std::string w_twitch::get_msg() {
    if (!endpoint.queue_empty(id)) {
        return endpoint.get_metadata(id)->pop();
    }
    return "";
}

void w_twitch::send_msg(std::string &msg) { endpoint.send(id, msg); }

bool w_twitch::update_keepalive(uint32_t timestamp) {
    if (keepalive == 0) {
        keepalive = timestamp;
        return true;
    } else {
        // Add a small timeout buffer cause ping exists
        if ((timestamp - keepalive) >= (keepalive_timeout - 2)) {
            keepalive = timestamp;
            return true;
        } else {
            // Reset for a reconnection
            keepalive = 0;
            return false;
        }
    }
}

void w_twitch::store_message(const std::string &msg, const std::string &usr) {
    chat_messages.push({msg, usr});
}

std::pair<std::string, std::string> w_twitch::get_message() {
    if (chat_messages.empty()) {
        return {"", ""};
    }
    std::pair front = chat_messages.front();
    chat_messages.pop();
    return front;
}
