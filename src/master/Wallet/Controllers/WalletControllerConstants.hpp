#pragma once

#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::master {

/** @brief What the answers of the routes of the wallet say when they
 * refuse. */
constexpr std::string_view INVALID_AMOUNT_DETAIL =
    "the amount must be 1 to 999999";
constexpr std::string_view NOT_ENOUGH_DETAIL =
    "the wallet holds less than the amount";
constexpr std::string_view PAST_THE_MAXIMUM_DETAIL =
    "the other balance would pass 999999";
constexpr std::string_view UNKNOWN_EXCHANGE_DETAIL =
    "this account made no exchange of that number";
constexpr std::string_view UNKNOWN_LOOK_DETAIL = "no look has that name";
constexpr std::string_view NOT_FOR_SALE_DETAIL = "the shop does not sell it";
constexpr std::string_view NOT_ENOUGH_KOPECKS_DETAIL =
    "the look costs more kopecks than the wallet holds";

/** @brief The numbers an exchange of the path may have: a line of the
 * journal, from the first. */
constexpr std::int64_t FIRST_EXCHANGE_ID = 1;
constexpr std::int64_t LAST_EXCHANGE_ID =
    std::numeric_limits<std::int64_t>::max();

}  // namespace rtype::master
