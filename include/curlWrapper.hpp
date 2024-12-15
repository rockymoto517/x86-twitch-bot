#pragma once

#include <optional>
#include <string>

namespace Curl {
const std::string LOGIN_URL_PREFIX = "https://api.twitch.tv/helix/users?login=";
std::optional<std::string> get_auth(const std::string &);
std::optional<std::string> login(const std::string &, const std::string &);
std::optional<std::string> subscribe(const std::string &, const std::string &,
                                     const std::string &);
std::optional<std::string> get_token(const std::string &, const std::string &);
std::optional<std::string> refresh_token(const std::string &,
                                         const std::string &,
                                         const std::string &);
};  // namespace Curl
