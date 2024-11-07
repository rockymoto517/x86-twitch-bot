#include "curlWrapper.hpp"

#include <curl/curl.h>
#include <fmt/os.h>
#include <fmt/printf.h>

namespace Curl {
void get_auth(const std::string &url) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
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

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fmt::print("Error performing get request:\n{}\n",
                       curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}
};  // namespace Curl
