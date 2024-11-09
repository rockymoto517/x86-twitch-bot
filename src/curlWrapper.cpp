#include "curlWrapper.hpp"

#include <curl/curl.h>
#include <fmt/format.h>
#include <fmt/printf.h>

#include <algorithm>
#include <nlohmann/json.hpp>

#include "constants.hpp"
#include "curl/easy.h"

#ifdef SET_CURL_VERBOSE
#define VERBOSE(c) curl_easy_setopt(c, CURLOPT_VERBOSE)
#else
#define VERBOSE(c) void()
#endif

namespace Curl {
using json = nlohmann::json;

size_t _response_callback(void *content, size_t size, size_t nmemb,
                          std::string *data) {
    size_t totalSize = size * nmemb;
    data->append(static_cast<char *>(content), totalSize);
    return totalSize;
}

std::optional<std::string> get_auth(const std::string &url) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        VERBOSE(curl);
        res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (res != CURLE_OK) {
            fmt::print("Error setting URL:\n{}\n", curl_easy_strerror(res));
        }
        // Cert verification
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying peer:\n{}\n", curl_easy_strerror(res));
        }
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying host:\n{}\n", curl_easy_strerror(res));
        }
        // Cache the verification for 1 week
        res = curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
        if (res != CURLE_OK) {
            fmt::print("Error setting cache timeout:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Setup callback to get response data
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set auth headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(
            headers,
            fmt::format("Authorization: OAuth {}", OAUTH_TOKEN).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fmt::print("Error performing get request:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return response;
        curl_global_cleanup();
    }
    curl_global_cleanup();
    return std::nullopt;
}

std::optional<std::string> login(const std::string &login) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        VERBOSE(curl);
        std::string url = LOGIN_URL_PREFIX + login;
        res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (res != CURLE_OK) {
            fmt::print("Error setting URL:\n{}\n", curl_easy_strerror(res));
        }
        // Cert verification
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying peer:\n{}\n", curl_easy_strerror(res));
        }
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying host:\n{}\n", curl_easy_strerror(res));
        }
        // Cache the verification for 1 week
        res = curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
        if (res != CURLE_OK) {
            fmt::print("Error setting cache timeout:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Setup callback to get response data
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set auth headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(
            headers,
            fmt::format("Authorization: Bearer {}", OAUTH_TOKEN).c_str());
        headers = curl_slist_append(
            headers, fmt::format("Client-Id: {}", BOT_CLIENT_ID).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fmt::print("Error performing get request:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return response;
        curl_global_cleanup();
    }
    curl_global_cleanup();
    return std::nullopt;
}

std::optional<std::string> subscribe(const std::string &session_id) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        VERBOSE(curl);
        res = curl_easy_setopt(curl, CURLOPT_URL, EVENTSUB_URL.c_str());
        if (res != CURLE_OK) {
            fmt::print("Error setting URL:\n{}\n", curl_easy_strerror(res));
        }
        // Cert verification
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying peer:\n{}\n", curl_easy_strerror(res));
        }
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying host:\n{}\n", curl_easy_strerror(res));
        }
        // Cache the verification for 1 week
        res = curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
        if (res != CURLE_OK) {
            fmt::print("Error setting cache timeout:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Setup callback to get response data
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set auth headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(
            headers,
            fmt::format("Authorization: Bearer {}", OAUTH_TOKEN).c_str());
        headers = curl_slist_append(
            headers, fmt::format("Client-Id: {}", BOT_CLIENT_ID).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Create EventSub body
        json events;
        events["type"] = "channel.chat.message";
        events["version"] = "1";
        events["condition"] = json(
            {{"broadcaster_user_id", CHANNEL_ID}, {"user_id", BOT_USER_ID}});
        events["transport"] =
            json({{"method", "websocket"}, {"session_id", session_id}});
        std::string dump = events.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dump.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dump.length());

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fmt::print("Error performing get request:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return response;
        curl_global_cleanup();
    }
    curl_global_cleanup();
    return std::nullopt;
}

std::optional<std::string> get_token() {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        VERBOSE(curl);
        res = curl_easy_setopt(curl, CURLOPT_URL,
                               "https://id.twitch.tv/oauth2/token");
        if (res != CURLE_OK) {
            fmt::print("Error setting URL:\n{}\n", curl_easy_strerror(res));
        }
        // Cert verification
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying peer:\n{}\n", curl_easy_strerror(res));
        }
        res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        if (res != CURLE_OK) {
            fmt::print("Error verifying host:\n{}\n", curl_easy_strerror(res));
        }
        // Cache the verification for 1 week
        res = curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
        if (res != CURLE_OK) {
            fmt::print("Error setting cache timeout:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Setup callback to get response data
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set auth headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(
            headers,
            fmt::format("Authorization: Bearer {}", OAUTH_TOKEN).c_str());
        headers = curl_slist_append(
            headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string body = fmt::format(
            "client_id={}&client_secret={}&grant_type=client_credentials",
            BOT_CLIENT_ID, SECRET);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fmt::print("Error performing get request:\n{}\n",
                       curl_easy_strerror(res));
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return response;
        curl_global_cleanup();
    }
    curl_global_cleanup();
    return std::nullopt;
}
};  // namespace Curl
