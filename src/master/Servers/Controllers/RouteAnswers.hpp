#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "KnownServer.hpp"
#include "NetworkException.hpp"
#include "PageRequest.hpp"
#include "QueryBounds.hpp"
#include "ServersService.hpp"

namespace rtype::master {

/** @returns An answer of @p status with no body: the problem document of
 * @p status, whose detail is @p detail. */
[[nodiscard]] network::ApiResponse refusal(int status, std::string_view detail);

/** @returns The answer to a request that carries no key the master issued:
 * 401, with `WWW-Authenticate: Bearer`. */
[[nodiscard]] network::ApiResponse unauthorizedResponse();

/** @returns The text of the parameter @p name of the query of @p request,
 * nothing when the request leaves it out. A parameter given twice keeps its
 * first value. */
[[nodiscard]] std::optional<std::string> queryTextOf(
    const network::ApiRequest& request, std::string_view name);

/** @returns The whole number @p text writes, nothing when it is no whole
 * number from bounds.lowest to bounds.highest; bounds.absent is not read. */
[[nodiscard]] std::optional<std::int64_t> boundedNumberIn(std::string_view text,
                                                          QueryBounds bounds);

/**
 * @returns The parameter @p name of the query of @p request: bounds.absent
 * when the request leaves it out, nothing when it is no whole number from
 * bounds.lowest to bounds.highest.
 */
[[nodiscard]] std::optional<std::int64_t> queryNumberOf(
    const network::ApiRequest& request, std::string_view name,
    QueryBounds bounds);

/** @returns The page the query of @p request asks for, nothing when its limit
 * or its offset is out of its bounds. */
[[nodiscard]] std::optional<PageRequest> pageOf(
    const network::ApiRequest& request);

/** @returns The answer of a page out of its bounds: 400, whose detail gives
 * the bounds. */
[[nodiscard]] network::ApiResponse invalidPageRefusal();

/** @returns What @p answer gives, or 400 naming the member it refused. */
template <typename Answer>
network::ApiResponse answerOrRefuse(Answer answer) {
  try {
    return answer();
  } catch (const network::InvalidMasterMessageException& invalid) {
    return refusal(BAD_REQUEST_STATUS, invalid.what());
  }
}

/**
 * @returns What @p answer gives for the server of the key of @p request: 401
 * when no server has that key, checked before the body is read, or 400 naming
 * the member @p answer refused.
 */
template <typename Answer>
network::ApiResponse answerAsServerOfKey(ServersService& service,
                                         const network::ApiRequest& request,
                                         Answer answer) {
  const std::optional<KnownServer> server =
      service.serverOfKey(request.bearerToken.value_or(std::string{}));
  if (!server.has_value()) {
    return unauthorizedResponse();
  }
  return answerOrRefuse([&answer, &server] { return answer(*server); });
}

}  // namespace rtype::master
