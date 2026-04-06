#pragma once

#include "curl_base.hpp"
#include "errors.hpp"

#include <expected>
#include <json.hpp>
#include <string>

namespace api {

class CostReport {
public:
    CostReport(const std::string &response);

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
