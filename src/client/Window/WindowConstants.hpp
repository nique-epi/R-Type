#pragma once

namespace rtype::client {

constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;
constexpr const char* WINDOW_TITLE = "R-Type";
constexpr unsigned int FRAMES_PER_SECOND_LIMIT = 60;

/**
 * @brief Extent of the whole window along one axis, as a fraction of itself.
 */
constexpr float WHOLE_WINDOW = 1.0F;

/**
 * @brief Share of the space the playfield leaves free that goes to each of the
 * two bars, so the playfield is centered.
 */
constexpr float BAR_SHARE_PER_SIDE = 0.5F;

}  // namespace rtype::client
