#include "query_costs_api.hpp"

#include "api_keys.hpp"

#include <chrono>
#include <fmt/core.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

std::string get_rfc_3339_timestamp_(const int days)
{
    const auto current_time = std::chrono::system_clock::now();
    const auto past_time = current_time - std::chrono::hours(days * 24);

    const std::time_t past_time_t = std::chrono::system_clock::to_time_t(past_time);
    const std::tm *gmt_time = std::gmtime(&past_time_t);

    // format to RFC 3339
    std::ostringstream oss;
    oss << std::put_time(gmt_time, "%Y-%m-%dT00:00:00Z");
    return oss.str();
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

    this->validate_schema_();
    this->raw_response = this->response_.dump(4);
}

void CostReport::validate_schema_()
{
    if (not this->response_.contains("data")) {
        throw std::runtime_error("Malformed models response. Missing 'data' key.");
    }

    if (not this->response_.contains("has_more")) {
        throw std::runtime_error("Malformed models response. Missing 'has_more' key.");
    }

    if (not this->response_.contains("next_page")) {
        throw std::runtime_error("Malformed models response. Missing 'next_page' key.");
    }
}

std::vector<CostBucket> CostReport::get_cost_buckets() const
{
    std::vector<CostBucket> buckets;

    for (const auto &d: this->response_["data"]) {
        buckets.emplace_back(d["results"][0]["amount"], d["starting_at"], d["ending_at"]);
    }

    return buckets;
}

std::expected<CostReport, Err> GetCosts::query_api(const int days)
{
    const std::string starting_at = get_rfc_3339_timestamp_(days);
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
