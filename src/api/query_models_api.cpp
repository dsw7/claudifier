#include "query_models_api.hpp"

#include "errors.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

std::string build_url_(const int limit, const std::optional<std::string> &last_id)
{
    std::string url;

    if (last_id) {
        url = fmt::format("https://api.anthropic.com/v1/models?limit={}&after_id={}", limit, *last_id);
    } else {
        url = fmt::format("https://api.anthropic.com/v1/models?limit={}", limit);
    }

    return url;
}

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

    if (json.contains("last_id")) {
        ok.last_id = json["last_id"];
    } else {
        throw std::runtime_error("Malformed models response. Missing 'last_id' key.");
    }

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

        ok.append_llm_model_to_page(item["created_at"], item["display_name"], item["id"]);
    }

    return ok;
}

} // namespace

namespace api {

void ModelsOutput::append_llm_model_to_page(const std::string &created_at, const std::string &display_name, const std::string &id)
{
    this->data.push_back(ModelData { created_at, display_name, id });
}

std::expected<ModelsOutput, Err> GetModels::query_api(const int limit, const std::optional<std::string> &last_id)
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
