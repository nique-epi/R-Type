#pragma once

#include "BiomeGround.hpp"

/** @returns The biome of one map 4 pixels wide the lookup tests read, built
 * from FLOOR_ROWS_OF_FOUR_COLUMNS and CEILING_ROWS_OF_FOUR_COLUMNS. */
[[nodiscard]] rtype::game::BiomeGround groundOfFourColumns();
