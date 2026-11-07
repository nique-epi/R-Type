#pragma once

#include <array>
#include <string_view>

/** @brief The folder of a resource inside a temporary folder, and its
 * manifest. */
constexpr std::string_view TEST_RESOURCE_NAME = "bydo-tyrant";
constexpr std::string_view TEST_MANIFEST = "bydo-tyrant/resource.toml";

/** @brief A manifest that describes itself and needs two resources. */
constexpr std::string_view FULL_MANIFEST =
    "description = \"The Bydo Tyrant and its brood\"\n"
    "depends = [\"crimson\", \"brood\"]\n";
constexpr std::string_view FULL_MANIFEST_DESCRIPTION =
    "The Bydo Tyrant and its brood";
constexpr std::array<std::string_view, 2> FULL_MANIFEST_DEPENDENCIES{"crimson",
                                                                     "brood"};

/** @brief A manifest that leaves out its dependencies. */
constexpr std::string_view MANIFEST_WITHOUT_DEPENDENCIES =
    "description = \"Alone\"\n";

/** @brief Manifests each wrong in one way. */
constexpr std::string_view MANIFEST_WITH_AN_UNKNOWN_KEY =
    "description = \"Alone\"\nversion = \"1.0\"\n";
constexpr std::string_view MANIFEST_WITH_DEPENDENCIES_AS_TEXT =
    "description = \"Alone\"\ndepends = \"crimson\"\n";
constexpr std::string_view MANIFEST_WITH_A_NUMBER_AS_DEPENDENCY =
    "description = \"Alone\"\ndepends = [4]\n";
constexpr std::string_view MANIFEST_WITH_AN_UPPERCASE_DEPENDENCY =
    "description = \"Alone\"\ndepends = [\"Crimson\"]\n";
constexpr std::string_view MANIFEST_WITHOUT_DESCRIPTION =
    "depends = [\"crimson\"]\n";
