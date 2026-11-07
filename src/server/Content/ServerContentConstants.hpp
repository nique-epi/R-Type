#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "RateLimit.hpp"

namespace rtype::server {

/**
 * @brief The downloads of the content pack one address may ask for: a client
 * asks once, then resumes a cut download up to twice, so three requests at
 * once, then one every ten seconds. Ten refused requests within a minute ban
 * the address for a minute. Each request can cost the supervisor
 * MAXIMUM_CONTENT_PACK_BYTES sent.
 */
constexpr std::size_t PACK_REQUEST_BURST = 3;
constexpr std::size_t PACK_REQUESTS_BEFORE_BAN = 10;
constexpr network::RateLimit PACK_REQUEST_RATE_LIMIT{
    .interval = std::chrono::seconds{10},
    .burst = PACK_REQUEST_BURST,
    .dropsBeforeBan = PACK_REQUESTS_BEFORE_BAN,
    .dropWindow = std::chrono::minutes{1},
    .banDuration = std::chrono::minutes{1}};

/** @brief The `detail` of the refusals of the route of the pack. */
constexpr std::string_view TOO_MANY_PACK_REQUESTS_DETAIL =
    "too many downloads of the content pack from this address";
constexpr std::string_view OTHER_PACK_DETAIL =
    "this server plays with another content pack";

/** @brief What an ETag puts around its value (RFC 9110). */
constexpr std::string_view ETAG_QUOTE = "\"";

}  // namespace rtype::server
