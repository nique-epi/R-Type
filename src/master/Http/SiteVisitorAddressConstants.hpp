#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::master {

/**
 * @brief The longest address the site may name its visitor by: the 45
 * characters of the longest IPv6 text, an IPv4 address mapped in IPv6
 * (INET6_ADDRSTRLEN of POSIX, without its end).
 */
constexpr std::size_t MAXIMUM_VISITOR_ADDRESS_LENGTH = 45;

/** @brief What separates the groups of an IPv6 address; an address has two at
 * least ("::1"). */
constexpr char IPV6_GROUP_SEPARATOR = ':';
constexpr std::ptrdiff_t FEWEST_IPV6_GROUP_SEPARATORS = 2;

/** @brief Every character an IPv6 address is written with, the dots of an
 * IPv4 address at its end included. */
constexpr std::string_view IPV6_ADDRESS_CHARACTERS = "0123456789abcdefABCDEF:.";

}  // namespace rtype::master
