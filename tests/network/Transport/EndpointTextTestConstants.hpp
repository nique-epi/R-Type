#pragma once

#include <array>
#include <cstdint>
#include <string_view>

/**
 * @brief A port written in decimal digits.
 */
constexpr std::string_view PORT_TEXT = "4243";

/**
 * @brief The port PORT_TEXT names.
 */
constexpr std::uint16_t PORT_OF_PORT_TEXT = 4243;

/**
 * @brief Texts made of digits whose value is above the highest port: one
 * past it, and one too large for any integer type.
 */
constexpr std::array<std::string_view, 2> PORTS_ABOVE_THE_HIGHEST{
    "65536", "99999999999999999999"};

/**
 * @brief Texts with a sign or a space around the digits of a port.
 */
constexpr std::array<std::string_view, 4> PORTS_WITH_SIGN_OR_SPACE{"-1", "+1",
                                                                   " 1", "1 "};

/**
 * @brief Texts where something that is not a decimal digit follows digits.
 */
constexpr std::array<std::string_view, 3> PORTS_FOLLOWED_BY_OTHER_TEXT{
    "1a", "0x10", "4242:4243"};

/**
 * @brief An IPv4 address of a machine on a local network.
 */
constexpr std::string_view LOCAL_NETWORK_ADDRESS = "192.168.1.20";

/**
 * @brief The IPv4 addresses made of the smallest and of the largest numbers.
 */
constexpr std::array<std::string_view, 2> IPV4_ADDRESSES_AT_THE_BOUNDS{
    "0.0.0.0", "255.255.255.255"};

/**
 * @brief Addresses with one number one above the largest, first or last.
 */
constexpr std::array<std::string_view, 2>
    IPV4_ADDRESSES_WITH_A_NUMBER_ABOVE_255{"256.0.0.1", "1.2.3.256"};

/**
 * @brief Addresses with one number too few and one too many.
 */
constexpr std::array<std::string_view, 2>
    IPV4_ADDRESSES_WITH_THREE_OR_FIVE_NUMBERS{"1.2.3", "1.2.3.4.5"};

/**
 * @brief Texts where one of the four numbers is missing: all of them, the
 * first, one in the middle, the last.
 */
constexpr std::array<std::string_view, 4> IPV4_ADDRESSES_WITH_AN_EMPTY_NUMBER{
    "", ".1.2.3", "1..2.3", "1.2.3."};

/**
 * @brief Addresses where a number other than 0 starts with 0.
 */
constexpr std::array<std::string_view, 2> IPV4_ADDRESSES_WITH_A_LEADING_ZERO{
    "01.2.3.4", "1.2.3.04"};

/**
 * @brief Addresses with a sign, a space or a port around the numbers.
 */
constexpr std::array<std::string_view, 4> IPV4_ADDRESSES_WITH_OTHER_TEXT{
    "+1.2.3.4", " 1.2.3.4", "1.2.3.4 ", "1.2.3.4:4242"};

/**
 * @brief A host name, which is never resolved.
 */
constexpr std::string_view HOST_NAME = "localhost";

/**
 * @brief The IPv6 address of the machine itself.
 */
constexpr std::string_view IPV6_LOOPBACK_ADDRESS = "::1";
