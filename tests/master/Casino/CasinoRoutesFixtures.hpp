#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include "RouteCall.hpp"
#include "UserCredentials.hpp"

/** @returns The credentials of R9, a second account of the tests. */
[[nodiscard]] rtype::network::UserCredentials secondPilotCredentials();

/** @returns The path of the bets of the account of a token on @p round of the
 * first room. */
[[nodiscard]] std::string ownBetsPathOf(std::uint64_t round);

/** @returns A `PUT` of @p body to @p path over HTTPS, with @p token as
 * Bearer. */
[[nodiscard]] RouteCall betsCall(std::string_view path, std::string_view token,
                                 std::string_view body);

/** @returns A body of @p count straight bets of one chip on 7. */
[[nodiscard]] std::string straightBetsOf(std::size_t count);
