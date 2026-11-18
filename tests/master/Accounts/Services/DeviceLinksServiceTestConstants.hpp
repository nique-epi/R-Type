#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The second a game asks for its link, and seconds measured from it:
 * the 10 minutes a link lives, and the 5 seconds a game waits between two
 * reads. */
constexpr std::chrono::sys_seconds LINK_OPENED_AT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds TEN_MINUTES_LATER =
    LINK_OPENED_AT + std::chrono::minutes{10};
constexpr std::chrono::sys_seconds FIVE_SECONDS_LATER =
    LINK_OPENED_AT + std::chrono::seconds{5};
constexpr std::chrono::sys_seconds THREE_SECONDS_LATER =
    LINK_OPENED_AT + std::chrono::seconds{3};
constexpr std::chrono::sys_seconds AN_HOUR_AND_A_SECOND_LATER =
    LINK_OPENED_AT + std::chrono::hours{1} + std::chrono::seconds{1};

/** @brief The address the games of the tests ask from, the links one address
 * may ask for in an hour, and the site the pages are on. */
constexpr std::string_view GAME_ADDRESS = "198.51.100.23";
constexpr std::size_t ALLOWED_LINKS_PER_HOUR = 10;
constexpr std::string_view TEST_SITE_URL = "https://site.test";
constexpr std::string_view TEST_LINK_PAGE = "https://site.test/link";
constexpr std::string_view CODE_QUERY_START = "https://site.test/link?code=";
