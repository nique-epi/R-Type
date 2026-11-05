#pragma once

#include <optional>
#include <string>

namespace rtype::master {

/** @brief What a route of the API reads of the request it answers. */
struct ApiRequest {
  /** @brief The body, MAX_REQUEST_BODY_BYTES at most. */
  std::string body;
  /**
   * @brief The token of an `Authorization: Bearer <token>` header (RFC 6750),
   * the scheme read in any case; empty without such a header.
   */
  std::optional<std::string> bearerToken = std::nullopt;
  /**
   * @brief The IPv4 address of the TCP peer, never one an `X-Forwarded-For`
   * header names.
   */
  std::string remoteAddress;
};

}  // namespace rtype::master
