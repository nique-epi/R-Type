#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "HttpHeader.hpp"
#include "MasterHttpConstants.hpp"

namespace rtype::network {

/** @brief What a route of the API answers: a status, a body and a header. */
struct ApiResponse {
  /** @brief The HTTP status, 200 when the route succeeded. */
  int status{};
  /**
   * @brief The body of the answer, sent as contentType. An error status
   * (LOWEST_ERROR_STATUS or more) with no body is sent as the problem document
   * of RFC 9457 of that status.
   */
  std::string body;
  /** @brief The `detail` of that problem document: what the client got wrong.
   */
  std::optional<std::string> detail = std::nullopt;
  /** @brief A header the answer carries, such as `Location` after a 201. */
  std::optional<HttpHeader> header = std::nullopt;
  /** @brief The media type of the body: JSON_CONTENT_TYPE but for a route
   * that answers a file, such as OCTET_STREAM_CONTENT_TYPE. */
  std::string_view contentType = JSON_CONTENT_TYPE;
};

}  // namespace rtype::network
