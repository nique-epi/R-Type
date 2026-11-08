#pragma once

#include <array>
#include <cstddef>
#include <string_view>

/** @brief The folder of the resources inside a temporary folder, and the
 * assets folder beside it. */
constexpr std::string_view TEST_RESOURCE_FOLDER = "resources";
constexpr std::string_view TEST_ASSET_FOLDER = "assets";

/** @brief Manifests of resources: one that needs nothing, one that needs the
 * first, and two that need each other. */
constexpr std::string_view MANIFEST_NEEDING_NOTHING =
    "description = \"Bydos of a crimson hue\"\n";
constexpr std::string_view MANIFEST_NEEDING_CRIMSON =
    "description = \"A boss and its brood\"\ndepends = [\"crimson\"]\n";
constexpr std::string_view MANIFEST_NEEDING_THE_EGG =
    "description = \"The hen\"\ndepends = [\"egg\"]\n";
constexpr std::string_view MANIFEST_NEEDING_THE_HEN =
    "description = \"The egg\"\ndepends = [\"hen\"]\n";

/** @brief The manifests of those resources, below the resource folder. */
constexpr std::string_view CRIMSON_MANIFEST = "resources/crimson/resource.toml";
constexpr std::string_view BOSS_MANIFEST = "resources/boss/resource.toml";
constexpr std::string_view HEN_MANIFEST = "resources/hen/resource.toml";
constexpr std::string_view EGG_MANIFEST = "resources/egg/resource.toml";

/** @brief The names of those resources. */
constexpr std::string_view CRIMSON_RESOURCE = "crimson";
constexpr std::string_view BOSS_RESOURCE = "boss";
constexpr std::string_view HEN_RESOURCE = "hen";
constexpr std::string_view MISSING_RESOURCE = "nowhere";

/** @brief Lines asking for the boss alone, then for the boss and its
 * dependency. */
constexpr std::array<std::string_view, 1> ONLY_THE_BOSS{BOSS_RESOURCE};
constexpr std::array<std::string_view, 2> THE_BOSS_THEN_CRIMSON{
    BOSS_RESOURCE, CRIMSON_RESOURCE};

/** @brief The folders laid for the boss: the assets, crimson, the boss. */
constexpr std::size_t FOLDERS_FOR_THE_BOSS = 3;

/** @brief A file the assets hold, that a resource may replace, and what each
 * copy holds. */
constexpr std::string_view REPLACEABLE_FILE = "waves/waves.toml";
constexpr std::string_view WAVES_OF_THE_ASSETS = "[difficulty.normal]\n";
constexpr std::string_view WAVES_OF_THE_RESOURCE = "[difficulty.crimson]\n";
