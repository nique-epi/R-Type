#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "GameException.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "SheetTextReadingTestConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::UnreadableDataFileException;
using rtype::game::InvalidEntityTypeNameException;
using rtype::game::NoEntitySheetException;
using rtype::game::SheetText;
using rtype::game::sheetTextsIn;
using rtype::game::TooManyEntityTypesException;
using rtype::game::testing::DRIFTER_FILE_NAME;
using rtype::game::testing::FILES_ONE_TOO_MANY;
using rtype::game::testing::FIRST_SHEET_NAME;
using rtype::game::testing::GENERATED_FILE_PREFIX;
using rtype::game::testing::MISSING_FOLDER_NAME;
using rtype::game::testing::OTHER_SHEET_TEXT;
using rtype::game::testing::README_FILE_NAME;
using rtype::game::testing::README_TEXT;
using rtype::game::testing::SECOND_SHEET_NAME;
using rtype::game::testing::SHEET_EXTENSION;
using rtype::game::testing::SHEETS_IN_THE_FOLDER;
using rtype::game::testing::SOME_SHEET_TEXT;
using rtype::game::testing::UPPERCASE_FILE_NAME;
using rtype::game::testing::WEAVER_FILE_NAME;

/**
 * Given a folder holding weaver.toml, drifter.toml and a README
 * When its sheets are read
 * Then the two sheets are read, drifter first, the order of their names, and
 * the README is left alone
 */
TEST(SheetTextReading, ReadsTheTomlFilesInTheOrderOfTheirNames) {
  const TemporaryFolder folder;
  folder.write(WEAVER_FILE_NAME, OTHER_SHEET_TEXT);
  folder.write(DRIFTER_FILE_NAME, SOME_SHEET_TEXT);
  folder.write(README_FILE_NAME, README_TEXT);

  const std::vector<SheetText> texts = sheetTextsIn(folder.path());

  ASSERT_EQ(texts.size(), SHEETS_IN_THE_FOLDER);
  EXPECT_EQ(texts.front().name, FIRST_SHEET_NAME);
  EXPECT_EQ(texts.back().name, SECOND_SHEET_NAME);
}

/**
 * Given a folder holding drifter.toml
 * When its sheets are read
 * Then the sheet holds the whole text of the file, and its errors name the
 * file by its path
 */
TEST(SheetTextReading, KeepsTheTextAndThePathOfEachFile) {
  const TemporaryFolder folder;
  folder.write(DRIFTER_FILE_NAME, SOME_SHEET_TEXT);

  const std::vector<SheetText> texts = sheetTextsIn(folder.path());

  ASSERT_EQ(texts.size(), 1U);
  EXPECT_EQ(texts.front().text, SOME_SHEET_TEXT);
  EXPECT_NE(texts.front().source.find(DRIFTER_FILE_NAME), std::string::npos);
}

/**
 * Given a folder that holds a README and no sheet
 * When its sheets are read
 * Then it is refused: a program that knows no type could draw nothing
 */
TEST(SheetTextReading, RefusesAFolderWithoutSheets) {
  const TemporaryFolder folder;
  folder.write(README_FILE_NAME, README_TEXT);

  EXPECT_THROW(static_cast<void>(sheetTextsIn(folder.path())),
               NoEntitySheetException);
}

/**
 * Given a folder that does not exist
 * When its sheets are read
 * Then it is refused, so a program without its sheets never starts
 */
TEST(SheetTextReading, RefusesAFolderThatCannotBeRead) {
  const TemporaryFolder folder;

  EXPECT_THROW(
      static_cast<void>(sheetTextsIn(folder.path() / MISSING_FOLDER_NAME)),
      UnreadableDataFileException);
}

/**
 * Given a folder holding Drifter.toml, with an uppercase letter
 * When its sheets are read
 * Then it is refused: a name is the same file on every system
 */
TEST(SheetTextReading, RefusesAFileNamedWithAnUppercaseLetter) {
  const TemporaryFolder folder;
  folder.write(UPPERCASE_FILE_NAME, SOME_SHEET_TEXT);

  EXPECT_THROW(static_cast<void>(sheetTextsIn(folder.path())),
               InvalidEntityTypeNameException);
}

/**
 * Given a folder holding 256 sheets, one more than a program loads
 * When its sheets are read
 * Then it is refused
 */
TEST(SheetTextReading, RefusesMoreSheetsThanAProgramLoads) {
  const TemporaryFolder folder;
  for (std::size_t i = 0; i < FILES_ONE_TOO_MANY; ++i) {
    folder.write(std::string{GENERATED_FILE_PREFIX} + std::to_string(i) +
                     std::string{SHEET_EXTENSION},
                 SOME_SHEET_TEXT);
  }

  EXPECT_THROW(static_cast<void>(sheetTextsIn(folder.path())),
               TooManyEntityTypesException);
}
