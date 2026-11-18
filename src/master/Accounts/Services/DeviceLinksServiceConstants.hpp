#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::master {

/**
 * @brief How long a device link waits for a player, and how often the game
 * reads it: the 10 minutes and 5 seconds of the examples of RFC 8628
 * (sections 3.2 and 3.5). A short life bounds the time a code read over
 * someone's shoulder is worth anything.
 */
constexpr std::chrono::seconds DEVICE_LINK_LIFETIME = std::chrono::minutes{10};
constexpr std::chrono::seconds DEVICE_LINK_INTERVAL = std::chrono::seconds{5};

/**
 * @brief The shortest time between two reads of a link by its game, a second
 * under DEVICE_LINK_INTERVAL: the master counts in whole seconds, so a game
 * that waits the interval may read 4.9 seconds after the last read by the
 * master's clock. A read sooner is refused with 429, RFC 8628's slow_down.
 */
constexpr std::chrono::seconds DEVICE_LINK_SHORTEST_READ_GAP =
    DEVICE_LINK_INTERVAL - std::chrono::seconds{1};

/**
 * @brief How many links one address may ask for in DEVICE_LINK_CREATION_WINDOW:
 * a player asks one per sign-in, and each is a row until it is forgotten, so
 * the quota bounds the rows a client can make the master keep.
 */
constexpr std::int64_t MAXIMUM_DEVICE_LINKS_PER_ADDRESS = 10;
constexpr std::chrono::seconds DEVICE_LINK_CREATION_WINDOW =
    std::chrono::hours{1};

/** @brief How long a link is kept once expired, so its game reads it expired
 * rather than unknown; longer than the creation window, so the quota still
 * counts it. */
constexpr std::chrono::seconds DEVICE_LINK_KEPT_AFTER_EXPIRY =
    std::chrono::hours{1};

/** @brief How many short codes are drawn for a new link before giving up: a
 * code another live link holds is drawn again, which 20 to the 8th codes make
 * rare. */
constexpr std::size_t MAXIMUM_USER_CODE_DRAWS = 8;

/** @brief The page of the web site that validates a link, after `site-url`,
 * and the query that names its short code. */
constexpr std::string_view DEVICE_LINK_PAGE_PATH = "/link";
constexpr std::string_view DEVICE_LINK_CODE_QUERY = "?code=";

/** @brief The words of the line a session opened through a link writes in
 * the journal; never its secret, its short code or the token. */
constexpr std::string_view SIGNED_IN_THROUGH_LINK_TEXT =
    " signed in through a device link from ";

}  // namespace rtype::master
