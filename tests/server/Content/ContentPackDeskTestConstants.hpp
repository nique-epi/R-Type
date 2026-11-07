#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::server::testing {

/** @brief Two addresses that ask for the pack. */
constexpr std::string_view PLAYER_ADDRESS = "203.0.113.7";
constexpr std::string_view OTHER_PLAYER_ADDRESS = "203.0.113.8";

/** @brief A fingerprint no pack of the tests has: 64 zeros. */
constexpr std::string_view UNKNOWN_FINGERPRINT =
    "0000000000000000000000000000000000000000000000000000000000000000";

/** @brief The requests one address may send at once, and the time after
 * which it may send one more, as the page of the protocol states them. */
constexpr std::size_t REQUESTS_AT_ONCE = 3;
constexpr std::chrono::seconds TIME_BETWEEN_REQUESTS{10};

/** @brief The statuses of RFC 9110 the desk answers. */
constexpr int OK_ANSWER = 200;
constexpr int NOT_FOUND = 404;
constexpr int TOO_MANY_REQUESTS = 429;

/** @brief What the desk answers a pack as, and the header of its version. */
constexpr std::string_view OCTET_STREAM = "application/octet-stream";
constexpr std::string_view ETAG = "ETag";
constexpr std::string_view ETAG_QUOTE = "\"";

}  // namespace rtype::server::testing
