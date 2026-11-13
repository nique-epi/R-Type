#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include "GlassStreak.hpp"
#include "RowWindow.hpp"
#include "ShipLookConstants.hpp"

namespace rtype::client {

/** @brief The locker: LOCKER_COLUMNS cells a row, LOCKER_ROWS rows shown,
 * under a name plate LOCKER_PLATE_HEIGHT tall, LOCKER_PADDING inside its sides.
 */
constexpr sf::Vector2i LOCKER_TOP_LEFT{24, 34};
constexpr int LOCKER_COLUMNS = 3;
constexpr int LOCKER_ROWS = 4;
constexpr int LOCKER_CELL_SIZE = 36;
constexpr int LOCKER_GAP = 5;
constexpr int LOCKER_PADDING = 10;
constexpr int LOCKER_PLATE_HEIGHT = 18;
constexpr sf::Vector2i LOCKER_SIZE{
    (2 * LOCKER_PADDING) + (LOCKER_COLUMNS * LOCKER_CELL_SIZE) +
        ((LOCKER_COLUMNS - 1) * LOCKER_GAP),
    LOCKER_PLATE_HEIGHT + (LOCKER_ROWS * LOCKER_CELL_SIZE) +
        ((LOCKER_ROWS - 1) * LOCKER_GAP) + LOCKER_PADDING};
constexpr std::size_t LOCKER_COLUMN_COUNT =
    static_cast<std::size_t>(LOCKER_COLUMNS);
constexpr std::size_t LOCKER_SHOWN_ROW_COUNT =
    static_cast<std::size_t>(LOCKER_ROWS);
constexpr std::size_t LOCKER_SLOT_COUNT =
    LOCKER_COLUMN_COUNT * LOCKER_SHOWN_ROW_COUNT;

/** @brief The rows the items fill, LOCKER_COLUMNS to a row, the last one
 * short when they do not fill it; the locker shows LOCKER_ROWS of them at a
 * time, the first ones when the hangar opens. */
constexpr std::size_t LOCKER_ITEM_ROW_COUNT =
    (identifiers::SHIP_LOOK_ITEM_COUNT + LOCKER_COLUMN_COUNT - 1) /
    LOCKER_COLUMN_COUNT;
constexpr RowWindow LOCKER_ROWS_AT_OPENING{.first = 0,
                                           .shown = LOCKER_SHOWN_ROW_COUNT,
                                           .count = LOCKER_ITEM_ROW_COUNT};

/** @brief The scroll mark of the locker: a track LOCKER_SCROLL_TRACK_WIDTH
 * wide on its right upright, LOCKER_SCROLL_TRACK_INSET from its right side,
 * between the glass door and the edge, as tall as the cells shown. */
constexpr int LOCKER_SCROLL_TRACK_WIDTH = 2;
constexpr int LOCKER_SCROLL_TRACK_INSET = 4;
constexpr sf::IntRect LOCKER_SCROLL_TRACK{
    {LOCKER_TOP_LEFT.x + LOCKER_SIZE.x - LOCKER_SCROLL_TRACK_INSET,
     LOCKER_TOP_LEFT.y + LOCKER_PLATE_HEIGHT},
    {LOCKER_SCROLL_TRACK_WIDTH,
     (LOCKER_ROWS * LOCKER_CELL_SIZE) + ((LOCKER_ROWS - 1) * LOCKER_GAP)}};

/** @brief The seeds of the strokes of the locker and of its plate, so they
 * look the same in every frame. */
constexpr std::uint32_t LOCKER_BODY_SEED = 11;
constexpr std::uint32_t LOCKER_WEAR_SEED = 12;
constexpr std::uint32_t NAMEPLATE_SEED = 401;

/** @brief The name plate of the locker: its name, its width beyond its name,
 * its height, where it and its name sit, its two brackets, and how far its
 * shade darkens its bottom row. */
constexpr const char* LOCKER_TITLE = "LOCKER";
constexpr int NAMEPLATE_MARGIN = 26;
constexpr int NAMEPLATE_MINIMUM_WIDTH = 70;
constexpr int NAMEPLATE_HEIGHT = 14;
constexpr int NAMEPLATE_TOP = 2;
constexpr int NAMEPLATE_TEXT_TOP = 3;
constexpr sf::Vector2i NAMEPLATE_BRACKET_SIZE{6, 8};
constexpr int NAMEPLATE_BRACKET_TOP = 3;
constexpr float NAMEPLATE_SHADE_SHARE = 0.45F;

/** @brief Inside a cell: the rows of shadow under its top, the shelf, the
 * icon area, the tag of a worn item, the frame of the chosen cell. */
constexpr int CELL_SHADOW_ROWS = 14;
constexpr float CELL_SHADOW_SHARE = 0.8F;
constexpr int CELL_SHELF_INSET = 6;
constexpr int CELL_SHELF_HEIGHT = 2;
constexpr int CELL_ICON_INSET = 1;
constexpr sf::Vector2i WORN_TAG_SIZE{5, 3};
constexpr int WORN_TAG_INSET = 3;
constexpr int CHOSEN_FRAME_GAP = 2;

/** @brief The reflections across the glass door, as the sketch draws them. */
constexpr GlassStreak WIDE_LEFT_STREAK{.start = 0.18F, .width = 3};
constexpr GlassStreak THIN_LEFT_STREAK{.start = 0.32F, .width = 1};
constexpr GlassStreak WIDE_RIGHT_STREAK{.start = 0.62F, .width = 2};
constexpr GlassStreak THIN_RIGHT_STREAK{.start = 0.78F, .width = 1};
constexpr std::array GLASS_STREAKS{WIDE_LEFT_STREAK, THIN_LEFT_STREAK,
                                   WIDE_RIGHT_STREAK, THIN_RIGHT_STREAK};
/** @brief A streak leans GLASS_STREAK_SLOPE pixel across per row; a pixel of
 * it lights where the dither threshold is under GLASS_STREAK_DENSITY. The
 * door stands GLASS_DOOR_INSET in from the sides; its handle. */
constexpr float GLASS_STREAK_SLOPE = 0.55F;
constexpr double GLASS_STREAK_DENSITY = 0.55;
constexpr int GLASS_DOOR_INSET = 5;
constexpr int GLASS_DOOR_RISE = 2;
constexpr sf::Vector2i GLASS_HANDLE_SIZE{3, 20};
constexpr int GLASS_HANDLE_INSET = 9;

}  // namespace rtype::client
