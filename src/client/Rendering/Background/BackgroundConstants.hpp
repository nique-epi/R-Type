#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <numbers>
#include <string_view>
#include "PixelGridConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Columns a plane of the sky keeps beyond each side of the background,
 * so a plane shifted by less than a pixel either way still covers it.
 */
constexpr int PLANE_MARGIN = 1;
constexpr sf::Vector2u PLANE_CANVAS_SIZE(
    BACKGROUND_SIZE.x + (2U * static_cast<unsigned int>(PLANE_MARGIN)),
    BACKGROUND_SIZE.y);

/** @brief The share of the size of a view between its center and each of
 * its edges. */
constexpr float VIEW_HALF_SHARE = 0.5F;

/**
 * @brief Longest time the background moves by in one frame.
 *
 * After a stall, such as the window being dragged, the sky resumes where it
 * was instead of jumping ahead.
 */
constexpr engine::Duration MAXIMUM_BACKGROUND_STEP =
    std::chrono::milliseconds(50);

/** @brief A whole turn, in radians. */
constexpr float FULL_TURN = 2.0F * std::numbers::pi_v<float>;
/**
 * @brief Halo: the background is blurred with a Gaussian of this standard
 * deviation, in background pixels, sampled over HALO_KERNEL_DEVIATIONS
 * standard deviations on each side.
 *
 * The design blurs by 4 pixels of a preview 640 pixels wide, which shows the
 * 480-pixel background: 3 background pixels.
 */
constexpr float HALO_STANDARD_DEVIATION = 3.0F;
constexpr float HALO_KERNEL_DEVIATIONS = 3.0F;
constexpr int HALO_KERNEL_RADIUS =
    static_cast<int>(HALO_KERNEL_DEVIATIONS * HALO_STANDARD_DEVIATION);

constexpr std::string_view BACKGROUND_LOGGER_NAME = "Background";

/** @brief The background the sky shows when a level names no biome, behind
 * the menus too, and for a biome it has no background for. */
constexpr std::string_view DEFAULT_BACKGROUND_NAME = "space";

/** @brief The layers behind the entrance of a background show from where the
 * entrance begins, fading in over SEAM_EDGE_WIDTH columns of dither. */
constexpr int SEAM_EDGE_WIDTH = 16;

/** @brief Once the entrance of a background begins left of this column, it
 * has left the screen with its fringe, whatever the shake of the camera, and
 * the background it replaced is no longer drawn. */
constexpr int ENTRANCE_GONE_COLUMN = -2 * SEAM_EDGE_WIDTH;

/** @brief A particle of a layer that warps at more than STREAK_SPEED_FACTOR
 * times its own speed is drawn with the streak its layer covers in one
 * SIMULATION_TICK_DURATION. */
constexpr float STREAK_SPEED_FACTOR = 1.5F;

}  // namespace rtype::client
