#include "RouteAnswers.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "PageRequest.hpp"
#include "QueryBounds.hpp"
#include "ServersControllerConstants.hpp"
#include "WholeNumberText.hpp"

namespace rtype::master {

network::ApiResponse refusal(int status, std::string_view detail) {
  return network::ApiResponse{
      .status = status, .body = {}, .detail = std::string{detail}};
}

network::ApiResponse unauthorizedResponse() {
  return network::ApiResponse{
      .status = UNAUTHORIZED_STATUS,
      .body = {},
      .detail = std::string{UNKNOWN_KEY_DETAIL},
      .header =
          network::HttpHeader{.name = std::string{WWW_AUTHENTICATE_HEADER},
                              .value = std::string{network::BEARER_SCHEME}}};
}

std::optional<std::string> queryTextOf(const network::ApiRequest& request,
                                       std::string_view name) {
  const auto parameter = request.queryParameters.find(std::string{name});
  if (parameter == request.queryParameters.end()) {
    return std::nullopt;
  }
  return parameter->second;
}

std::optional<std::int64_t> boundedNumberIn(std::string_view text,
                                            QueryBounds bounds) {
  const std::optional<std::uint64_t> number =
      network::wholeNumberIn<std::uint64_t>(text);
  if (!number.has_value() || std::cmp_less(*number, bounds.lowest) ||
      std::cmp_greater(*number, bounds.highest)) {
    return std::nullopt;
  }
  return static_cast<std::int64_t>(*number);
}

std::optional<std::int64_t> queryNumberOf(const network::ApiRequest& request,
                                          std::string_view name,
                                          QueryBounds bounds) {
  const std::optional<std::string> text = queryTextOf(request, name);
  if (!text.has_value()) {
    return bounds.absent;
  }
  return boundedNumberIn(*text, bounds);
}

std::optional<PageRequest> pageOf(const network::ApiRequest& request) {
  const std::optional<std::int64_t> limit =
      queryNumberOf(request, network::LIMIT_QUERY_PARAMETER,
                    QueryBounds{.lowest = network::MINIMUM_PAGE_SIZE,
                                .highest = network::MAXIMUM_PAGE_SIZE,
                                .absent = network::DEFAULT_PAGE_SIZE});
  const std::optional<std::int64_t> offset =
      queryNumberOf(request, network::OFFSET_QUERY_PARAMETER,
                    QueryBounds{.lowest = network::FIRST_PAGE_OFFSET,
                                .highest = network::HIGHEST_PAGE_OFFSET,
                                .absent = network::FIRST_PAGE_OFFSET});
  if (!limit.has_value() || !offset.has_value()) {
    return std::nullopt;
  }
  return PageRequest{.limit = *limit, .offset = *offset};
}

network::ApiResponse invalidPageRefusal() {
  return refusal(BAD_REQUEST_STATUS,
                 std::string{INVALID_LIMIT_DETAIL_START} +
                     std::to_string(network::MINIMUM_PAGE_SIZE) +
                     std::string{DETAIL_RANGE_JOIN} +
                     std::to_string(network::MAXIMUM_PAGE_SIZE) +
                     std::string{INVALID_OFFSET_DETAIL_START} +
                     std::to_string(network::FIRST_PAGE_OFFSET));
}

}  // namespace rtype::master
