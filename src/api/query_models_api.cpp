#include "query_models_api.hpp"

#include "parse_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

using OptStr = std::optional<std::string>;

ModelListModelsResult unpack_200_response_to_model_(const std::string &response)
{
    const nlohmann::json json = api::parse_response(response);

    /*
    if (json["type"] != "message") {
        throw std::runtime_error("Malformed message response. Response is not a message");
    }
    */

    ModelListModelsResult ok;
    ok.raw_response = json.dump(4);
    return ok;
}

std::string build_url_(const int limit, const OptStr &last_id)
{
    std::string url;

    if (last_id) {
        url = fmt::format("https://api.anthropic.com/v1/models?limit={}&last_id={}", limit, *last_id);
    } else {
        url = fmt::format("https://api.anthropic.com/v1/models?limit={}", limit);
    }

    return url;
}

} // namespace

namespace api {

std::expected<ModelListModelsResult, Err> GetModels::query_api(const int limit, const OptStr &last_id)
{
    const std::string endpoint = build_url_(limit, last_id);
    curl_easy_setopt(this->curl_, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->curl_, CURLOPT_HTTPGET, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + this->user_api_key_).c_str());
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, headers);

    std::string response;
    curl_easy_setopt(this->curl_, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(this->curl_);

    curl_slist_free_all(headers);
    headers = nullptr;

    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }

    long http_status_code = -1;
    curl_easy_getinfo(this->curl_, CURLINFO_RESPONSE_CODE, &http_status_code);

    if (http_status_code == 200) {
        return unpack_200_response_to_model_(response);
    }

    return std::unexpected(unpack_error(response));
}

} // namespace api
