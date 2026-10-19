#pragma once

#include <cstdint>
#include <string_view>

/**
 * @brief A port given after `-p`, written in digits.
 */
constexpr std::string_view PORT_ARGUMENT = "4243";

/**
 * @brief The port PORT_ARGUMENT names.
 */
constexpr std::uint16_t PORT_OF_PORT_ARGUMENT = 4243;

/**
 * @brief A second port given after `-p`, and the port it names.
 */
constexpr std::string_view OTHER_PORT_ARGUMENT = "4244";

/**
 * @brief The port OTHER_PORT_ARGUMENT names.
 */
constexpr std::uint16_t PORT_OF_OTHER_PORT_ARGUMENT = 4244;

/**
 * @brief A value after `-p` that is no port.
 */
constexpr std::string_view NOT_A_PORT_ARGUMENT = "abc";

/**
 * @brief An option no program of the repository defines.
 */
constexpr std::string_view UNKNOWN_OPTION = "-x";

/**
 * @brief The client's option for the server address, which the server does
 * not take.
 */
constexpr std::string_view ADDRESS_OPTION = "-h";

/**
 * @brief The line of the usage that gives the default port.
 */
constexpr std::string_view DEFAULT_PORT_LINE =
    "UDP port to listen on, from 1 to 65535. Default: 4242.";

/**
 * @brief Seeds given after `--seed`, and the numbers they name.
 */
constexpr std::string_view SEED_ARGUMENT = "1234";
constexpr std::uint32_t SEED_OF_SEED_ARGUMENT = 1234;
constexpr std::string_view OTHER_SEED_ARGUMENT = "5678";
constexpr std::uint32_t SEED_OF_OTHER_SEED_ARGUMENT = 5678;
constexpr std::string_view LOWEST_SEED_ARGUMENT = "0";
constexpr std::string_view HIGHEST_SEED_ARGUMENT = "4294967295";
constexpr std::uint32_t HIGHEST_SEED = 4294967295U;

/**
 * @brief Values after `--seed` that are not seeds: one above the highest, a
 * negative number, nothing.
 */
constexpr std::string_view TOO_HIGH_SEED_ARGUMENT = "4294967296";
constexpr std::string_view NEGATIVE_SEED_ARGUMENT = "-1";
constexpr std::string_view EMPTY_SEED_ARGUMENT{};
