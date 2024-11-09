#pragma once

#include <queue>
#include <utility>

#include "client.hpp"

class w_twitch {
   private:
    websocket_endpoint endpoint;
    std::string channel_name;
    std::string channel_id;
    int id;
    uint32_t keepalive = 0;
    std::queue<std::pair<std::string, std::string>> chat_messages;

   public:
    std::string session_id;
    uint32_t keepalive_timeout;

    w_twitch(const std::string &name);
    ~w_twitch();
    std::string get_msg();
    void send_msg(std::string &);
    void end_threads();
    bool update_keepalive(uint32_t);
    void store_message(const std::string &, const std::string &);
    std::pair<std::string, std::string> get_message();
};
