#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "WholeNumberRange.hpp"

namespace rtype::client {

/**
 * @brief The two tables the root of an entity sheet holds: the server reads
 * the first, the client the second. Any other key of the root is refused.
 */
constexpr std::string_view GAMEPLAY_TABLE_KEY = "gameplay";
constexpr std::string_view APPEARANCE_TABLE_KEY = "appearance";
constexpr std::array SHEET_ROOT_KEYS{GAMEPLAY_TABLE_KEY, APPEARANCE_TABLE_KEY};

/**
 * @brief The keys of `[appearance]`: the asset id of its sprite sheet, and
 * the table of its idle animation. Each is required, any other is refused.
 */
constexpr std::string_view SHEET_KEY = "sheet";
constexpr std::string_view IDLE_TABLE_KEY = "idle";
constexpr std::array APPEARANCE_KEYS{SHEET_KEY, IDLE_TABLE_KEY};

/**
 * @brief The keys of `[appearance.idle]`: the first frame, in pixels of the
 * sheet, how many frames follow it in its row, the pixels between two of
 * them, and how long each shows, in milliseconds. Each is required, any other
 * is refused.
 */
constexpr std::string_view FRAME_LEFT_KEY = "left";
constexpr std::string_view FRAME_TOP_KEY = "top";
constexpr std::string_view FRAME_WIDTH_KEY = "width";
constexpr std::string_view FRAME_HEIGHT_KEY = "height";
constexpr std::string_view FRAME_COUNT_KEY = "frames";
constexpr std::string_view FRAME_SPACING_KEY = "spacing";
constexpr std::string_view FRAME_TIME_KEY = "frame_ms";
constexpr std::array IDLE_KEYS{
    FRAME_LEFT_KEY,  FRAME_TOP_KEY,     FRAME_WIDTH_KEY, FRAME_HEIGHT_KEY,
    FRAME_COUNT_KEY, FRAME_SPACING_KEY, FRAME_TIME_KEY};

/**
 * @brief The largest side of a sprite sheet the client reads, in pixels:
 * 4096, the texture size every graphics card of the last decade takes.
 */
constexpr std::int64_t LARGEST_SHEET_PIXELS = 4096;

/** @brief The most frames an idle animation holds. */
constexpr std::int64_t MOST_IDLE_FRAMES = 16;

/** @brief The most pixels between two frames of a row. */
constexpr std::int64_t WIDEST_FRAME_SPACING = 64;

/** @brief The longest a frame shows, in milliseconds: a minute. */
constexpr std::int64_t LONGEST_FRAME_MILLISECONDS = 60000;

/** @brief What each key of `[appearance.idle]` accepts. */
constexpr engine::WholeNumberRange FRAME_CORNER_RANGE{
    .lowest = 0, .highest = LARGEST_SHEET_PIXELS - 1};
constexpr engine::WholeNumberRange FRAME_SIDE_RANGE{
    .lowest = 1, .highest = LARGEST_SHEET_PIXELS};
constexpr engine::WholeNumberRange FRAME_COUNT_RANGE{
    .lowest = 1, .highest = MOST_IDLE_FRAMES};
constexpr engine::WholeNumberRange FRAME_SPACING_RANGE{
    .lowest = 0, .highest = WIDEST_FRAME_SPACING};
constexpr engine::WholeNumberRange FRAME_TIME_RANGE{
    .lowest = 1, .highest = LONGEST_FRAME_MILLISECONDS};

}  // namespace rtype::client
