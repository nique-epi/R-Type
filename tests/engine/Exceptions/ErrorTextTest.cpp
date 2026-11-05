#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "DataFilePlace.hpp"
#include "ErrorText.hpp"
#include "ErrorTextTestConstants.hpp"

using rtype::engine::DataFilePlace;
using rtype::engine::listText;
using rtype::engine::placeText;
using rtype::engine::rangeText;

/**
 * Given a place on line 2, column 8 of sheet.toml
 * When it is written
 * Then it reads as compilers write a place: the file, the line, the column
 */
TEST(ErrorText, APlaceReadsAsACompilerWritesIt) {
  const DataFilePlace place{.file = std::string{SHEET_FILE},
                            .line = SIDE_LINE,
                            .column = SIDE_COLUMN,
                            .key = std::string{SIDE_KEY_NAME}};

  EXPECT_EQ(placeText(place), SIDE_PLACE_TEXT);
}

/**
 * Given the names player and enemy
 * When they are written as a list
 * Then they are separated by a comma, in their order
 */
TEST(ErrorText, NamesAreSeparatedByCommas) {
  const std::vector<std::string> names{std::string{PLAYER}, std::string{ENEMY}};

  EXPECT_EQ(listText(names), TWO_NAMES_TEXT);
}

/**
 * Given the choices ship, steady and still, as views of a constants header
 * When they are written as a list
 * Then they read as a list of names does, separated by commas, in their order
 */
TEST(ErrorText, ChoicesAreWrittenAsNames) {
  EXPECT_EQ(listText(THREE_CHOICES), THREE_CHOICES_TEXT);
}

/**
 * Given no name
 * When it is written as a list
 * Then the list says none, never an empty text
 */
TEST(ErrorText, NoNameSaysNone) {
  EXPECT_EQ(listText(std::vector<std::string>{}), NO_NAME_TEXT);
}

/**
 * Given the whole numbers from 1 to 100
 * When they are written
 * Then they read 1 to 100, both bounds named
 */
TEST(ErrorText, ARangeNamesBothBounds) {
  EXPECT_EQ(rangeText(ONE_TO_A_HUNDRED), ONE_TO_A_HUNDRED_TEXT);
}
