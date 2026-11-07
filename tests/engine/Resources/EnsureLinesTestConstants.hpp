#pragma once

#include <array>
#include <string_view>

/** @brief A configuration that starts two resources, names the first twice
 * and holds other commands between them. */
constexpr std::array<std::string_view, 5> LINES_STARTING_TWO_RESOURCES{
    "set port 4242", "ensure bydo-tyrant", "# ensure commented-out",
    "ensure crimson_drifter", "ensure bydo-tyrant"};
constexpr std::array<std::string_view, 2> RESOURCES_OF_THOSE_LINES{
    "bydo-tyrant", "crimson_drifter"};

/** @brief `ensure` lines that do not name exactly one resource. */
constexpr std::array<std::string_view, 1> ENSURE_WITHOUT_NAME{"ensure"};
constexpr std::array<std::string_view, 1> ENSURE_WITH_TWO_NAMES{
    "ensure bydo-tyrant crimson_drifter"};

/** @brief A line that names a resource with an uppercase letter. */
constexpr std::array<std::string_view, 1> ENSURE_WITH_UPPERCASE{
    "ensure Bydo-Tyrant"};

/** @brief The words of an `ensure` line and of a `set` line. */
constexpr std::array<std::string_view, 2> ENSURE_WORDS{"ensure", "bydo-tyrant"};
constexpr std::array<std::string_view, 3> SET_WORDS{"set", "port", "4242"};
