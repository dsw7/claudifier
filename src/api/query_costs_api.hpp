#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
#include <json.hpp>
#include <string>
#include <vector>

namespace api {

struct CostBucket {
    CostBucket(const std::string &starting_at, const std::string &ending_at, const std::string &amount) :
        starting_at(starting_at), ending_at(ending_at), amount(amount) {}

    std::string starting_at;
    std::string ending_at;
    std::string amount;
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
