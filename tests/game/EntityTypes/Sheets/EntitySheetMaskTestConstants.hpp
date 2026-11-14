#pragma once

#include <cstddef>
#include <string_view>
#include "MaskPixel.hpp"

namespace rtype::game::testing {

/** @brief A mask of 3 by 3 pixels drawing a cross, and pixels of it: a drawn
 * one of the top row, a clear corner, and a drawn one of the middle row. */
constexpr std::string_view MASK_OF_A_CROSS =
    "[gameplay.mask]\nrows = [\".#.\", \"###\", \".#.\"]\n";
constexpr int CROSS_SIDE = 3;
constexpr engine::MaskPixel TOP_OF_THE_CROSS{.x = 1, .y = 0};
constexpr engine::MaskPixel CORNER_OF_THE_CROSS{.x = 0, .y = 0};
constexpr engine::MaskPixel RIGHT_ARM_OF_THE_CROSS{.x = 2, .y = 1};

/** @brief The same cross, its outline peeled once by the tool that wrote it. */
constexpr std::string_view MASK_OF_A_PEELED_CROSS =
    "[gameplay.mask]\nerosion = 1\nrows = [\".#.\", \"###\", \".#.\"]\n";
constexpr int ONE_PIXEL_PEELED = 1;

/** @brief Masks refused, and the texts of their refusals. */
constexpr std::string_view MASK_OF_A_SHORT_ROW =
    "[gameplay.mask]\nrows = [\"###\", \"##\"]\n";
constexpr std::string_view MASK_OF_ANOTHER_CHARACTER =
    "[gameplay.mask]\nrows = [\"#x#\"]\n";
constexpr std::string_view MASK_OF_UNKNOWN_CHARACTERS_ONLY =
    "[gameplay.mask]\nrows = [\"xxx\"]\n";
constexpr std::string_view MASK_OF_NO_ROW = "[gameplay.mask]\nrows = []\n";
constexpr std::string_view MASK_THAT_DRAWS_NOTHING =
    "[gameplay.mask]\nrows = [\"...\"]\n";
constexpr std::string_view MASK_OF_AN_UNKNOWN_KEY =
    "[gameplay.mask]\ncolour = 1\nrows = [\"#\"]\n";
constexpr std::string_view MASK_OF_TOO_MUCH_EROSION =
    "[gameplay.mask]\nerosion = 5\nrows = [\"#\"]\n";
constexpr std::string_view MASK_BESIDE_BOXES =
    "[gameplay.mask]\nrows = [\"#\"]\n";
constexpr std::string_view MASK_ROWS_DOTTED_KEY = "gameplay.mask.rows";
constexpr std::string_view MASK_COLOUR_DOTTED_KEY = "gameplay.mask.colour";
constexpr std::string_view MASK_DOTTED_KEY = "gameplay.mask";
constexpr std::string_view MASK_SIDE_RULE_TEXT =
    "a mask holds 1 to 256 rows of 1 to 256 pixels";
constexpr std::string_view MASK_ROW_WIDTH_RULE_TEXT =
    "every row of a mask is as wide as its first row";
constexpr std::string_view MASK_PIXEL_RULE_TEXT =
    "a row of a mask holds only # and .";
constexpr std::string_view MASK_DRAWN_RULE_TEXT =
    "a mask draws at least one pixel";
constexpr std::string_view MASK_AND_PARTS_RULE_TEXT =
    "a sheet gives a mask or several hitbox boxes, never both";

/** @brief The widest row a mask may hold, and one pixel more. */
constexpr std::size_t WIDEST_MASK_ROW = 256;
constexpr std::size_t ROW_PAST_THE_WIDEST = 257;

}  // namespace rtype::game::testing
