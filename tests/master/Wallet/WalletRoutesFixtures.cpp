#include "WalletRoutesFixtures.hpp"
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "RouteCall.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "WalletRoutesTestConstants.hpp"

namespace {

/** @returns The member @p member of the JSON object @p answer holds, nothing
 * when it holds none. */
std::optional<nlohmann::json> memberOfAnswer(const httplib::Result& answer,
                                             std::string_view member) {
  if (!answer) {
    return std::nullopt;
  }
  nlohmann::json document = nlohmann::json::parse(answer->body, nullptr, false);
  if (!document.is_object() || !document.contains(member)) {
    return std::nullopt;
  }
  return std::move(document.at(member));
}

}  // namespace

RouteCall ownCall(std::string_view method, std::string_view path,
                  std::string_view token) {
  return RouteCall{.method = method,
                   .path = path,
                   .key = token,
                   .body = {},
                   .overHttps = true};
}

RouteCall exchangeCall(std::string_view token, std::string body) {
  return RouteCall{.method = POST_METHOD,
                   .path = OWN_EXCHANGES_URL,
                   .key = token,
                   .body = std::move(body),
                   .overHttps = true};
}

std::optional<std::int64_t> numberIn(const httplib::Result& answer,
                                     std::string_view member) {
  const std::optional<nlohmann::json> value = memberOfAnswer(answer, member);
  if (!value.has_value() || !value->is_number_integer()) {
    return std::nullopt;
  }
  return value->get<std::int64_t>();
}

std::optional<std::size_t> arraySizeIn(const httplib::Result& answer,
                                       std::string_view member) {
  const std::optional<nlohmann::json> value = memberOfAnswer(answer, member);
  if (!value.has_value() || !value->is_array()) {
    return std::nullopt;
  }
  return value->size();
}
