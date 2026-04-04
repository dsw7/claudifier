#include "query_models_api.hpp"

#include "errors.hpp"

#include <fmt/core.h>
#include <stdexcept>

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

    this->is_200_response_();
    const CURLcode code = curl_easy_perform(this->curl_);

    curl_slist_free_all(headers);
    headers = nullptr;

    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }

    if (this->is_200_response_()) {
        ModelsOutput output(response);
        return output;
    }

    return std::unexpected(unpack_error(response));
}

void ModelsOutput::validate_schema_()
{
    if (not this->response_.contains("has_more")) {
        throw std::runtime_error("Malformed models response. Missing 'has_more' key.");
    }

    if (not this->response_.contains("data")) {
        throw std::runtime_error("Malformed models response. Missing 'data' key.");
    }
}

ModelsOutput::ModelsOutput(const std::string &response)
{
    try {
        this->response_ = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    this->validate_schema_();

    this->raw_response = this->response_.dump(4);
    this->has_more = this->response_["has_more"];

    for (const auto &model: this->response_["data"]) {
        if (model["type"] != "model") {
            throw std::runtime_error("Malformed models response. Object in 'data' array is not a model.");
        }

        this->models.push_back(Model { model["created_at"], model["display_name"], model["id"] });
    }
}

} // namespace api
