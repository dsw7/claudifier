#include "query_costs_api.hpp"

#include "api_keys.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace {

std::string get_rfc_3339_timestamp_()
{
    return "2026-04-05T00:00:00Z";
}

} // namespace

namespace api {

CostReport::CostReport(const std::string &response)
{
    try {
        this->response_ = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    this->raw_response = this->response_.dump(4);
}

std::expected<CostReport, Err> GetCosts::query_api()
{
    const std::string starting_at = get_rfc_3339_timestamp_();
    const std::string endpoint = fmt::format("https://api.anthropic.com/v1/organizations/cost_report?starting_at={}", starting_at);
    curl_easy_setopt(this->curl_, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->curl_, CURLOPT_HTTPGET, 1L);

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-Api-Key: " + get_admin_api_key()).c_str());
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

    if (this->is_200_response_()) {
        return CostReport(response);
    }

    return std::unexpected(Err(response));
}

} // namespace api
