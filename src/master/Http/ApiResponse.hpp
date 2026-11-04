#pragma once

#include <optional>
#include <string>
#include "HttpHeader.hpp"

namespace rtype::master {

/** @brief What a route of the API answers: a status, a body and a header. */
struct ApiResponse {
  /** @brief The HTTP status, OK_STATUS when the route succeeded. */
  int status{};
  /**
   * @brief The JSON text of the answer, sent as network::JSON_CONTENT_TYPE. An
   * error status (network::LOWEST_ERROR_STATUS or more) with no body is sent as
   * the problem document of RFC 9457 of that status.
   */
  std::string body;
  /** @brief The `detail` of that problem document: what the client got wrong.
   */
  std::optional<std::string> detail = std::nullopt;
  /** @brief A header the answer carries, such as `Location` after a 201. */
  std::optional<HttpHeader> header = std::nullopt;
};

}  // namespace rtype::master
