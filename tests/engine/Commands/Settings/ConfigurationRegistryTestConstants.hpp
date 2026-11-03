#pragma once

#include <cstdint>
#include <string_view>

/**
 * @brief The name of the whole-number setting the tests declare.
 */
constexpr std::string_view PORT_SETTING = "port";

/**
 * @brief The lowest value of the port setting.
 */
constexpr std::int64_t LOWEST_PORT_VALUE = 1024;

/**
 * @brief The highest value of the port setting.
 */
constexpr std::int64_t HIGHEST_PORT_VALUE = 65535;

/**
 * @brief The value of the port setting before any `set`.
 */
constexpr std::int64_t DEFAULT_PORT_VALUE = 4242;

/**
 * @brief The name of the setting that may hold nothing.
 */
constexpr std::string_view SEED_SETTING = "seed";

/**
 * @brief The name of the text setting the tests declare.
 */
constexpr std::string_view NICKNAME_SETTING = "nickname";

/**
 * @brief The value of the nickname setting before any `set`.
 */
constexpr std::string_view DEFAULT_NICKNAME = "pilot";

/**
 * @brief A nickname the validator accepts.
 */
constexpr std::string_view ACCEPTED_NICKNAME = "ace";

/**
 * @brief A nickname the validator refuses.
 */
constexpr std::string_view REFUSED_NICKNAME = "forbidden";

/**
 * @brief The reason the validator gives for REFUSED_NICKNAME.
 */
constexpr std::string_view NICKNAME_REFUSAL = "that nickname is taken";

/**
 * @brief A setting no test declares.
 */
constexpr std::string_view UNDECLARED_SETTING = "volume";

/**
 * @brief A port in bounds, different from the default.
 */
constexpr std::string_view PORT_TEXT_IN_BOUNDS = "6000";

/**
 * @brief The port PORT_TEXT_IN_BOUNDS writes.
 */
constexpr std::int64_t PORT_VALUE_IN_BOUNDS = 6000;

/**
 * @brief The lowest port, as text.
 */
constexpr std::string_view PORT_TEXT_LOWEST = "1024";

/**
 * @brief The highest port, as text.
 */
constexpr std::string_view PORT_TEXT_HIGHEST = "65535";

/**
 * @brief One below the lowest port, as text.
 */
constexpr std::string_view PORT_TEXT_BELOW_LOWEST = "1023";

/**
 * @brief One above the highest port, as text.
 */
constexpr std::string_view PORT_TEXT_ABOVE_HIGHEST = "65536";

/**
 * @brief A value that is not a whole number.
 */
constexpr std::string_view NOT_A_NUMBER = "abc";

/**
 * @brief A number far larger than any setting can hold.
 */
constexpr std::string_view NUMBER_TOO_LARGE = "99999999999999999999999";

/**
 * @brief The word that clears a setting.
 */
constexpr std::string_view CLEARING_WORD = "none";

/**
 * @brief A seed, as text, the tests give.
 */
constexpr std::string_view SEED_TEXT = "7";

/**
 * @brief The bounds of the port setting as the refusal message writes them.
 */
constexpr std::string_view PORT_BOUNDS_TEXT = "1024..65535";
