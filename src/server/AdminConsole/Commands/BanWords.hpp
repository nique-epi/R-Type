#pragma once

#include <chrono>
#include <optional>
#include <string_view>
#include "BanTarget.hpp"

namespace rtype::server {

/**
 * @returns The target @p word names: an address when it reads as an IPv4
 * address (see network::ipv4AddressFromText()), a nickname when it is one the
 * protocol accepts (see network::isValidNickname()); nothing otherwise. A
 * nickname may hold digits and dots, so "01.2.3.4", which is no address, is a
 * nickname.
 */
[[nodiscard]] std::optional<BanTarget> banTargetOf(std::string_view word);

/**
 * @returns The length @p word gives: a count from 1 to MAXIMUM_BAN_COUNT in
 * decimal digits, then MINUTE_LETTER, HOUR_LETTER or DAY_LETTER; nothing for
 * any other word, a sign, a space or a missing letter included.
 */
[[nodiscard]] std::optional<std::chrono::seconds> banLengthOf(
    std::string_view word);

}  // namespace rtype::server
