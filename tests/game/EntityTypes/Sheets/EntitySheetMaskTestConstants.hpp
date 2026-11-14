#pragma once

#include <cstddef>
#include <string_view>
#include "MaskPixel.hpp"

namespace rtype::game::testing {

/** @brief Tables `[gameplay.mask]` of the sheet READ_SHEET_NAME: naming its
 * mask file, the same peeled once by the tool that wrote it, naming the mask
 * file of another sheet, with a key no mask has, and peeled 5 times. */
constexpr std::string_view MASK_TABLE =
    "[gameplay.mask]\nfile = \"masks/test_sheet.txt\"\n";
constexpr std::string_view MASK_TABLE_PEELED_ONCE =
    "[gameplay.mask]\nfile = \"masks/test_sheet.txt\"\nerosion = 1\n";
constexpr std::string_view MASK_TABLE_OF_ANOTHER_SHEET =
    "[gameplay.mask]\nfile = \"masks/drifter.txt\"\n";
constexpr std::string_view MASK_TABLE_OF_AN_UNKNOWN_KEY =
    "[gameplay.mask]\nfile = \"masks/test_sheet.txt\"\ncolour = 1\n";
constexpr std::string_view MASK_TABLE_OF_TOO_MUCH_EROSION =
    "[gameplay.mask]\nfile = \"masks/test_sheet.txt\"\nerosion = 5\n";
constexpr int ONE_PIXEL_PEELED = 1;

/** @brief A mask file of 3 by 3 pixels drawing a cross, the same written
 * with Windows line endings, and pixels of it: a drawn one of the top row, a
 * clear corner, and a drawn one of the middle row. */
constexpr std::string_view MASK_OF_A_CROSS = ".#.\n###\n.#.\n";
constexpr std::string_view MASK_OF_A_CROSS_WITH_CARRIAGE_RETURNS =
    ".#.\r\n###\r\n.#.\r\n";
constexpr int CROSS_SIDE = 3;
constexpr engine::MaskPixel TOP_OF_THE_CROSS{.x = 1, .y = 0};
constexpr engine::MaskPixel CORNER_OF_THE_CROSS{.x = 0, .y = 0};
constexpr engine::MaskPixel RIGHT_ARM_OF_THE_CROSS{.x = 2, .y = 1};

/** @brief Mask files refused, and where their refusals point: the second
 * line of the file, its first line, and its first line again. */
constexpr std::string_view MASK_OF_A_SHORT_SECOND_ROW = "###\n##\n";
constexpr std::string_view MASK_OF_ANOTHER_CHARACTER = "#x#\n";
constexpr std::string_view MASK_OF_UNKNOWN_CHARACTERS_ONLY = "xxx\n";
constexpr std::string_view MASK_OF_NO_ROW{};
constexpr std::string_view MASK_THAT_DRAWS_NOTHING = "...\n";
constexpr std::string_view MASK_OF_ONE_PIXEL = "#\n";
constexpr std::string_view SECOND_LINE_OF_THE_MASK =
    "/sheets/masks/test_sheet.txt:2:1";
constexpr std::string_view FIRST_LINE_OF_THE_MASK =
    "/sheets/masks/test_sheet.txt:1:1";

/** @brief The dotted keys of the refusals, and the texts of their rules. */
constexpr std::string_view MASK_FILE_DOTTED_KEY = "gameplay.mask.file";
constexpr std::string_view MASK_COLOUR_DOTTED_KEY = "gameplay.mask.colour";
constexpr std::string_view MASK_DOTTED_KEY = "gameplay.mask";
constexpr std::string_view MASK_FILE_RULE_TEXT =
    "a mask is the file masks/<the name of the sheet>.txt next to the sheet";
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
