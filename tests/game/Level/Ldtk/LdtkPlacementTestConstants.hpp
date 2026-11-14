#pragma once

#include <string_view>

/** @brief The sheet every enemy the level tests place takes, and its width:
 * the 96 by 96 collision box of the shipped Bydos. */
constexpr std::string_view PLACED_DRIFTER_SHEET = "drifter";
constexpr float PLACED_ENEMY_WIDTH = 96.0F;

/** @brief The name of the LDtk file the level tests write, below a content
 * folder. */
constexpr std::string_view TEST_LDTK_FILE = "levels/test.ldtk";
