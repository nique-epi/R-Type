#pragma once

#include <array>
#include <string_view>
#include "EntityTypesTestConstants.hpp"

/** @brief The example resource the repository ships, and the folder of the
 * resources, from the assets folder. */
constexpr std::string_view EXAMPLE_RESOURCE_NAME = "crimson-drifter";
constexpr std::string_view RESOURCE_FOLDER_FROM_THE_ASSETS = "../resources";

/** @brief The sheet of the crimson Drifter the example resource brings. */
constexpr std::string_view CRIMSON_DRIFTER_SHEET = "crimson_drifter";

/** @brief The enemies of normal once the example resource is started: those
 * its waves file gives, the crimson Drifter third and last. */
constexpr std::array<std::string_view, 7> EXAMPLE_RESOURCE_NORMAL_ENEMIES{
    rtype::game::testing::DRIFTER_SHEET,
    rtype::game::testing::WEAVER_SHEET,
    CRIMSON_DRIFTER_SHEET,
    rtype::game::testing::VOLTJELLY_SHEET,
    rtype::game::testing::WEAVER_SHEET,
    rtype::game::testing::REAPER_SHEET,
    CRIMSON_DRIFTER_SHEET};
