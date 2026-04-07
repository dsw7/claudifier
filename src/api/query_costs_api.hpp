#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
#include <json.hpp>
#include <string>
#include <vector>

namespace api {

struct CostBucket {
    CostBucket(const std::string &amount, const std::string &starting_at, const std::string &ending_at) :
        amount(amount), starting_at(starting_at), ending_at(ending_at) {}

    std::string amount;
    std::string starting_at;
    std::string ending_at;
};

class CostReport {
public:
    CostReport(const std::string &response);
    std::vector<CostBucket> get_cost_buckets() const;

    bool has_more = false;
    std::string raw_response;

private:
    nlohmann::json response_;
    void validate_schema_();
};

class GetCosts: public CurlBase {
public:
    std::expected<CostReport, Err> query_api(const int days);
};

} // namespace api
