#pragma once

#include <chrono>
#include <cstdint>
#include "LayerFrame.hpp"

/** @brief The seed the tests of the layers give their random draws. */
constexpr std::uint32_t LAYER_SEED = 2026;

/** @brief A layer that has not moved, one that moved 10 columns to the left,
 * and one whose particles leave a streak of 3 pixels in a warp. */
constexpr rtype::client::LayerFrame AT_ITS_ORIGIN{};
constexpr rtype::client::LayerFrame TEN_COLUMNS_LEFT{.column = -10};
constexpr int TEN_COLUMNS = 10;
constexpr rtype::client::LayerFrame IN_A_WARP{.column = 0, .streak = 3};
constexpr int STREAK_OF_THE_WARP = 3;

/** @brief Times the tests move a layer on by. */
constexpr std::chrono::milliseconds ONE_FRAME_OF_LAYER{50};
constexpr std::chrono::seconds ONE_SECOND_OF_LAYER{1};
constexpr std::chrono::seconds TEN_SECONDS_OF_LAYER{10};

/** @brief The size of the background, in background pixels. */
constexpr float WIDTH_OF_THE_BACKGROUND = 480.0F;
constexpr float HEIGHT_OF_THE_BACKGROUND = 270.0F;
