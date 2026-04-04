#include "query_models_api.hpp"

#include "errors.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

api::ModelsOutput unpack_200_response_to_model_(const std::string &response)
{
    nlohmann::json json;

    try {
        json = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    api::ModelsOutput ok;
    ok.raw_response = json.dump(4);

    if (json.contains("has_more")) {
        ok.has_more = json["has_more"];
    } else {
        throw std::runtime_error("Malformed models response. Missing 'has_more' key.");
    }

    if (not json.contains("data")) {
        throw std::runtime_error("Malformed models response. Missing 'data' key.");
    }

    for (const auto &item: json["data"]) {
        if (item["type"] != "model") {
            throw std::runtime_error("Malformed models response. Object in 'data' array is not a model.");
        }

        ok.data.push_back(api::ModelData { item["created_at"], item["display_name"], item["id"] });
    }

    return ok;
}

} // namespace

namespace api {

std::expected<ModelsOutput, Err> GetModels::query_api()
{
    static int limit = 1000;
    const std::string endpoint = fmt::format("https://api.anthropic.com/v1/models?limit={}", limit);

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
