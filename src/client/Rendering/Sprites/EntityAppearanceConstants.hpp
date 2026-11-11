#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "EntitySheetConstants.hpp"
#include "Playback.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::client {

/**
 * @brief The keys of `[appearance]`: the asset id of its sprite sheet, and
 * the table of its idle animation. Each is required. Any other key is
 * refused, but `attack_<n>`, the table of the attack numbered n, which is
 * optional: ATTACK_TABLE_PREFIX followed by a number of ATTACK_NUMBER_RANGE.
 */
constexpr std::string_view SHEET_KEY = game::SPRITE_SHEET_KEY;
constexpr std::string_view IDLE_TABLE_KEY = "idle";
constexpr std::string_view ATTACK_TABLE_PREFIX = "attack_";
constexpr std::array APPEARANCE_KEYS{SHEET_KEY, IDLE_TABLE_KEY};

/** @brief The optional keys of `[appearance]`: `enraged_idle`, the idle
 * animation of an enemy once enraged, and `death`, the death played instead of
 * the Bydo explosion, whose version without flash lies in the row right under
 * it, each read like `idle` (IDLE_KEYS); then `effects` and `looks`, a table
 * per name (EFFECT_KEYS, IDLE_KEYS). */
constexpr std::string_view ENRAGED_IDLE_TABLE_KEY = "enraged_idle";
constexpr std::string_view DEATH_TABLE_KEY = "death";
constexpr std::array OPTIONAL_APPEARANCE_KEYS{
    ENRAGED_IDLE_TABLE_KEY, DEATH_TABLE_KEY, game::EFFECTS_TABLE_KEY,
    game::LOOKS_TABLE_KEY};

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
 * @brief The keys of `[appearance.attack_<n>.<phase>]`: the frames as in the
 * idle table, then `playback` (required), `afterwards` and the two anchors,
 * which the table may leave out. Any other key is refused.
 */
constexpr std::string_view PLAYBACK_KEY = "playback";
constexpr std::string_view AFTERWARDS_KEY = "afterwards";
constexpr std::string_view ANCHOR_X_KEY = "anchor_x";
constexpr std::string_view ANCHOR_Y_KEY = "anchor_y";
constexpr std::array ACTION_KEYS{
    FRAME_LEFT_KEY,  FRAME_TOP_KEY,     FRAME_WIDTH_KEY, FRAME_HEIGHT_KEY,
    FRAME_COUNT_KEY, FRAME_SPACING_KEY, FRAME_TIME_KEY,  PLAYBACK_KEY,
    AFTERWARDS_KEY,  ANCHOR_X_KEY,      ANCHOR_Y_KEY};

/**
 * @brief The keys of `[appearance.effects.<name>]`: the frames as in the idle
 * table, whose version without flash lies in the row right under them, and
 * the two anchors, which the table may leave out. A table of
 * `[appearance.looks]` takes the keys of the idle table (IDLE_KEYS). Any other
 * key is refused.
 */
constexpr std::array EFFECT_KEYS{
    FRAME_LEFT_KEY,   FRAME_TOP_KEY,   FRAME_WIDTH_KEY,
    FRAME_HEIGHT_KEY, FRAME_COUNT_KEY, FRAME_SPACING_KEY,
    FRAME_TIME_KEY,   ANCHOR_X_KEY,    ANCHOR_Y_KEY};

/** @brief The names `playback` accepts, and the Playback of each, in the same
 * order. */
constexpr std::array PLAYBACK_NAMES{std::string_view{"loop"},
                                    std::string_view{"once"},
                                    std::string_view{"hold"}};
constexpr std::array PLAYBACKS{Playback::Loop, Playback::Once, Playback::Hold};
static_assert(PLAYBACK_NAMES.size() == PLAYBACKS.size(),
              "one name per playback");

/** @brief The names `afterwards` accepts: `idle` gives the body back its idle
 * animation, `stay` keeps the last frame. `idle` is the first, and the
 * default. */
constexpr std::string_view RETURNS_TO_IDLE = "idle";
constexpr std::string_view STAYS_ON_LAST_FRAME = "stay";
constexpr std::array AFTERWARDS_NAMES{RETURNS_TO_IDLE, STAYS_ON_LAST_FRAME};

/**
 * @brief The largest side of a sprite sheet the client reads, in pixels:
 * 4096, the texture size every graphics card of the last decade takes.
 */
constexpr std::int64_t LARGEST_SHEET_PIXELS = 4096;

/** @brief The most frames an animation of a sheet holds. */
constexpr std::int64_t MOST_FRAMES = 16;

/** @brief The farthest a frame hangs from the body, in pixels of the canvas,
 * either way. */
constexpr std::int64_t FARTHEST_ANCHOR = 256;

/** @brief The most pixels between two frames of a row. */
constexpr std::int64_t WIDEST_FRAME_SPACING = 64;

/** @brief The longest a frame shows, in milliseconds: a minute. */
constexpr std::int64_t LONGEST_FRAME_MILLISECONDS = 60000;

/** @brief What each key of `[appearance.idle]` and of an attack phase
 * accepts. */
constexpr engine::WholeNumberRange FRAME_CORNER_RANGE{
    .lowest = 0, .highest = LARGEST_SHEET_PIXELS - 1};
constexpr engine::WholeNumberRange FRAME_SIDE_RANGE{
    .lowest = 1, .highest = LARGEST_SHEET_PIXELS};
constexpr engine::WholeNumberRange FRAME_COUNT_RANGE{.lowest = 1,
                                                     .highest = MOST_FRAMES};
constexpr engine::WholeNumberRange FRAME_SPACING_RANGE{
    .lowest = 0, .highest = WIDEST_FRAME_SPACING};
constexpr engine::WholeNumberRange FRAME_TIME_RANGE{
    .lowest = 1, .highest = LONGEST_FRAME_MILLISECONDS};
constexpr engine::WholeNumberRange ANCHOR_RANGE{.lowest = -FARTHEST_ANCHOR,
                                                .highest = FARTHEST_ANCHOR};

}  // namespace rtype::client
