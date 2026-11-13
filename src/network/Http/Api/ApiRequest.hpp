#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace rtype::network {

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
   * @brief The IPv4 address of the client: the TCP peer, or the one a proxy
   * on the machine names, as clientAddressOf() reads it.
   */
  std::string remoteAddress;
  /**
   * @brief The headers of the request, their names in lowercase, since a
   * header name is read in any case (RFC 9110, section 5.1); a header given
   * twice keeps one of its values. Whatever the client wrote: a route trusts
   * none of them without a check of its own.
   */
  std::unordered_map<std::string, std::string> headers;
  /**
   * @brief The parameters the path of the route names, such as `id` for
   * `/servers/:id/games`, as the text the client wrote; none for a path
   * without parameter.
   */
  std::unordered_map<std::string, std::string> pathParameters;
  /**
   * @brief The parameters of the query of the path, such as `limit` for
   * `/servers?limit=20`, as the text the client wrote; a parameter given twice
   * keeps its first value. None for a path without query.
   */
  std::unordered_map<std::string, std::string> queryParameters;
};

}  // namespace rtype::network
