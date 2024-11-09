#include <fmt/printf.h>

#include <nlohmann/json.hpp>

#include "botFuncs.hpp"
#include "constants.hpp"
#include "curlWrapper.hpp"

using json = nlohmann::json;
static w_twitch *bot = nullptr;

void main_loop() {
    bot = new w_twitch("rockytestdll3");
    while (true) {
        if (!Bot::do_loop(bot)) {
            fmt::print("Disconnected.\n");
            delete bot;
            break;
        } else {
            auto [usr, msg] = bot->get_message();
            if (usr != "") {
                fmt::print("{}: {}\n", msg, usr);
            }
        }
    }
}

int main() {
    try {
        Curl::refresh_token();
        std::optional<std::string> res = Curl::get_auth(AUTH_URL);
        if (res) {
            json resJson = json::parse(res.value());
            if (resJson.contains("status")) {
                uint16_t status = resJson["status"].template get<uint16_t>();
                fmt::print("Status code: {}\n", status);
            }
        }
        // std::thread loop_thread(main_loop);
        main_loop();

    } catch (std::exception &e) {
        fmt::print("Exception in main: {}\n", e.what());
    }
}
