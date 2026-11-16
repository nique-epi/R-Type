#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "DatedRequest.hpp"
#include "HttpConstants.hpp"
#include "ISessionAccounts.hpp"
#include "KnownServer.hpp"
#include "NetworkException.hpp"
#include "PageRequest.hpp"
#include "QueryBounds.hpp"
#include "ServersControllerConstants.hpp"
#include "ServersService.hpp"
#include "User.hpp"

namespace rtype::master {

/** @returns An answer of @p status with no body: the problem document of
 * @p status, whose detail is @p detail. */
[[nodiscard]] network::ApiResponse refusal(int status, std::string_view detail);

/** @returns The answer to a request that carries no key or token the master
 * issued: 401, with `WWW-Authenticate: Bearer`, whose detail is @p detail. */
[[nodiscard]] network::ApiResponse unauthorizedResponse(
    std::string_view detail);

/** @returns The second the master answers at: the clock of the system, its
 * part of a second dropped. */
[[nodiscard]] std::chrono::sys_seconds currentSecond();

/** @returns The text of the parameter @p name of the path of @p request,
 * nothing when the path names none. */
[[nodiscard]] std::optional<std::string> pathTextOf(
    const network::ApiRequest& request, std::string_view name);

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
    return unauthorizedResponse(UNKNOWN_KEY_DETAIL);
  }
  return answerOrRefuse([&answer, &server] { return answer(*server); });
}

/**
 * @returns What @p answer gives for the account of the session of the token
 * of @p dated, read at its date: 401, with `WWW-Authenticate: Bearer`, when
 * the request carries no token or no session has it, checked before the body
 * is read; or 400 naming the member @p answer refused.
 */
template <typename Answer>
network::ApiResponse answerAsAccountOf(ISessionAccounts& accounts,
                                       const DatedRequest& dated,
                                       Answer answer) {
  const std::optional<network::User> user = accounts.userOfToken(
      dated.request.get().bearerToken.value_or(std::string{}), dated.now);
  if (!user.has_value()) {
    return unauthorizedResponse(NO_SESSION_DETAIL);
  }
  return answerOrRefuse([&answer, &user] { return answer(*user); });
}

}  // namespace rtype::master
