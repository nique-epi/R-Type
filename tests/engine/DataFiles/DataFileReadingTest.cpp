#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include <toml++/toml.hpp>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileReadingTestConstants.hpp"
#include "DataFileTable.hpp"
#include "DataFileText.hpp"
#include "EngineException.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::choiceAt;
using rtype::engine::dataFileContent;
using rtype::engine::dataFilesIn;
using rtype::engine::DataFileSyntaxException;
using rtype::engine::DataFileTable;
using rtype::engine::DataFileTableExpectedException;
using rtype::engine::DataFileTablesExpectedException;
using rtype::engine::DataFileText;
using rtype::engine::DataFileTextExpectedException;
using rtype::engine::DataFileTextsExpectedException;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::keyName;
using rtype::engine::millisecondsAt;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::nodeAt;
using rtype::engine::optionalTableAt;
using rtype::engine::parseDataFile;
using rtype::engine::refuseUnknownKeys;
using rtype::engine::rootOf;
using rtype::engine::tableAt;
using rtype::engine::tablesAt;
using rtype::engine::textAt;
using rtype::engine::textsAt;
using rtype::engine::UnknownDataFileChoiceException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::engine::UnreadableDataFileException;
using rtype::engine::wholeNumberAt;
using rtype::engine::wholeNumberOrZeroAt;
using ::testing::HasSubstr;
using ::testing::StartsWith;
using ::testing::StrEq;
using ::testing::ThrowsMessage;

/**
 * Given a data file whose line 2 has a key and no value
 * When it is read
 * Then a DataFileSyntaxException names the file and line 2
 */
TEST(DataFileReading, TextThatIsNotTomlIsRefusedAtItsLine) {
  EXPECT_THAT(
      [] {
        static_cast<void>(parseDataFile(NOT_TOML_ON_LINE_2, SHEET_FILE_NAME));
      },
      ThrowsMessage<DataFileSyntaxException>(
          StartsWith(std::string{SYNTAX_ERROR_START})));
}

/**
 * Given the root of a data file
 * When one of its keys is named
 * Then the name is the key alone
 */
TEST(DataFileReading, TheRootNamesAKeyByItself) {
  const toml::table root = parseDataFile(A_SIDE, SHEET_FILE_NAME);

  EXPECT_EQ(keyName(rootOf(root, SHEET_FILE_NAME), SIDE_KEY), SIDE_KEY);
}

/**
 * Given the table gameplay.hitbox of a data file
 * When its key width is named
 * Then the name is the dotted name of the table, then the key
 */
TEST(DataFileReading, ATableNamesItsKeysWithItsDottedName) {
  const toml::table root = parseDataFile(A_HITBOX, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_EQ(keyName(tableAt(gameplay, HITBOX_TABLE), WIDTH_KEY),
            WIDTH_OF_THE_HITBOX);
}

/**
 * Given a table gameplay that holds no pilot
 * When its pilot is read
 * Then a MissingDataFileKeyException names gameplay.pilot
 */
TEST(DataFileReading, AMissingKeyIsNamed) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT([&gameplay] { static_cast<void>(nodeAt(gameplay, PILOT_KEY)); },
              ThrowsMessage<MissingDataFileKeyException>(
                  HasSubstr(std::string{MISSING_PILOT_TEXT})));
}

/**
 * Given a table gameplay whose line 3 holds a misspelt key
 * When only side is known
 * Then an UnknownDataFileKeyException names the key, its line and its column
 */
TEST(DataFileReading, AMisspeltKeyIsRefusedAtTheKey) {
  const toml::table root =
      parseDataFile(GAMEPLAY_WITH_A_MISSPELT_KEY, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT([&gameplay] { refuseUnknownKeys(gameplay, SIDE_ONLY); },
              ThrowsMessage<UnknownDataFileKeyException>(
                  StrEq(std::string{MISSPELT_KEY_MESSAGE})));
}

/**
 * Given a table gameplay that holds side alone
 * When only side is known
 * Then nothing is refused
 */
TEST(DataFileReading, KnownKeysAreAccepted) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_NO_THROW(refuseUnknownKeys(gameplay, SIDE_ONLY));
}

/**
 * Given a table gameplay whose hitbox is a number
 * When the hitbox is read as a table
 * Then a DataFileTableExpectedException names gameplay.hitbox
 */
TEST(DataFileReading, AValueWhereATableIsExpectedIsRefused) {
  const toml::table root =
      parseDataFile(HITBOX_THAT_IS_A_NUMBER, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT(
      [&gameplay] { static_cast<void>(tableAt(gameplay, HITBOX_TABLE)); },
      ThrowsMessage<DataFileTableExpectedException>(
          HasSubstr(std::string{HITBOX_NOT_A_TABLE_TEXT})));
}

/**
 * Given a table gameplay that holds no gun
 * When its gun is read as a table it may leave out
 * Then there is none
 */
TEST(DataFileReading, AnAbsentOptionalTableIsNothing) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_FALSE(optionalTableAt(gameplay, GUN_TABLE).has_value());
}

/**
 * Given a table gameplay.gun
 * When the gun of gameplay is read as a table it may leave out
 * Then it is that table, named gameplay.gun
 */
TEST(DataFileReading, APresentOptionalTableIsReadWithItsName) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_GUN, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  const std::optional<DataFileTable> gun = optionalTableAt(gameplay, GUN_TABLE);

  ASSERT_TRUE(gun.has_value());
  EXPECT_EQ(gun.value_or(gameplay).name, NAME_OF_THE_GUN);
}

/**
 * Given a table gameplay whose gun is a number
 * When its gun is read as a table it may leave out
 * Then a DataFileTableExpectedException is thrown: a key that is there must
 * hold a table
 */
TEST(DataFileReading, AnOptionalTableHoldingAValueIsRefused) {
  const toml::table root = parseDataFile(GUN_THAT_IS_A_NUMBER, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THROW(static_cast<void>(optionalTableAt(gameplay, GUN_TABLE)),
               DataFileTableExpectedException);
}

/**
 * Given a table gameplay with two tables in the array box
 * When the boxes are read
 * Then there are two, in the order of the file, named after their index
 */
TEST(DataFileReading, AnArrayOfTablesIsReadInTheOrderOfTheFile) {
  const toml::table root =
      parseDataFile(GAMEPLAY_WITH_TWO_BOXES, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  const std::vector<DataFileTable> boxes = tablesAt(gameplay, BOX_KEY);

  ASSERT_EQ(boxes.size(), TWO_BOXES);
  EXPECT_EQ(boxes.front().name, NAME_OF_THE_FIRST_BOX);
  EXPECT_EQ(boxes.back().name, NAME_OF_THE_SECOND_BOX);
  EXPECT_EQ(wholeNumberAt(boxes.front(), WIDTH_KEY, BOX_WIDTH_RANGE),
            WIDTH_OF_THE_FIRST_BOX_VALUE);
  EXPECT_EQ(wholeNumberAt(boxes.back(), WIDTH_KEY, BOX_WIDTH_RANGE),
            WIDTH_OF_THE_SECOND_BOX_VALUE);
}

/**
 * Given a table gameplay whose second box has a key out of its range
 * When the width of the second box is read
 * Then the error names gameplay.box[1].width
 */
TEST(DataFileReading, AnErrorInAnArrayElementNamesItsIndex) {
  const toml::table root =
      parseDataFile(GAMEPLAY_WITH_TWO_BOXES, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);
  const std::vector<DataFileTable> boxes = tablesAt(gameplay, BOX_KEY);

  EXPECT_THAT(
      [&boxes] {
        static_cast<void>(
            wholeNumberAt(boxes.back(), WIDTH_KEY, NARROW_BOX_WIDTH_RANGE));
      },
      ThrowsMessage<DataFileValueOutOfRangeException>(
          HasSubstr(std::string{WIDTH_OF_THE_SECOND_BOX})));
}

/**
 * Given a table gameplay whose array box is empty
 * When the boxes are read
 * Then there is none
 */
TEST(DataFileReading, AnEmptyArrayHoldsNoTable) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_NO_BOX, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_TRUE(tablesAt(gameplay, BOX_KEY).empty());
}

/**
 * Given a table gameplay with one table box, not an array
 * When the boxes are read
 * Then a DataFileTablesExpectedException names gameplay.box
 */
TEST(DataFileReading, ASingleTableIsNotAnArrayOfTables) {
  const toml::table root =
      parseDataFile(GAMEPLAY_WITH_ONE_TABLE_BOX, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT([&gameplay] { static_cast<void>(tablesAt(gameplay, BOX_KEY)); },
              ThrowsMessage<DataFileTablesExpectedException>(
                  HasSubstr(std::string{BOXES_NOT_TABLES_TEXT})));
}

/**
 * Given a table gameplay whose array box holds a table and a number
 * When the boxes are read
 * Then a DataFileTablesExpectedException is thrown
 */
TEST(DataFileReading, ANumberAmongTheTablesIsRefused) {
  const toml::table root =
      parseDataFile(GAMEPLAY_WITH_A_NUMBER_AMONG_BOXES, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THROW(static_cast<void>(tablesAt(gameplay, BOX_KEY)),
               DataFileTablesExpectedException);
}

/**
 * Given a table gameplay with no box
 * When the boxes are read
 * Then a MissingDataFileKeyException is thrown
 */
TEST(DataFileReading, AnAbsentArrayOfTablesIsMissing) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THROW(static_cast<void>(tablesAt(gameplay, BOX_KEY)),
               MissingDataFileKeyException);
}

/**
 * Given the health 1 and the health 1000, the two bounds of 1 to 1000
 * When each is read in that range
 * Then each is read as it is written
 */
TEST(DataFileReading, AWholeNumberIsReadAtBothBounds) {
  const toml::table root =
      parseDataFile(BOTH_BOUNDS_OF_HEALTH, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_EQ(wholeNumberAt(gameplay, LOWEST_KEY, HEALTH_RANGE), LOWEST_HEALTH);
  EXPECT_EQ(wholeNumberAt(gameplay, HIGHEST_KEY, HEALTH_RANGE), HIGHEST_HEALTH);
}

/**
 * Given a table that holds the health 1 and leaves the key health out
 * When each key is read as a key the file may omit
 * Then the health written is read as it is, and the key left out reads 0
 */
TEST(DataFileReading, AWholeNumberLeftOutReadsZero) {
  const toml::table root =
      parseDataFile(BOTH_BOUNDS_OF_HEALTH, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_EQ(wholeNumberOrZeroAt(gameplay, LOWEST_KEY, HEALTH_RANGE),
            LOWEST_HEALTH);
  EXPECT_EQ(wholeNumberOrZeroAt(gameplay, HEALTH_KEY, HEALTH_RANGE), 0);
}

/**
 * Given the health 0, one under 1 to 1000
 * When it is read in that range
 * Then a DataFileValueOutOfRangeException names the file, line 2, the column
 * of the value, the key and its range
 */
TEST(DataFileReading, AWholeNumberUnderItsRangeIsRefusedAtTheValue) {
  const toml::table root = parseDataFile(NO_HEALTH, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT(
      [&gameplay] {
        static_cast<void>(wholeNumberAt(gameplay, HEALTH_KEY, HEALTH_RANGE));
      },
      ThrowsMessage<DataFileValueOutOfRangeException>(
          StrEq(std::string{NO_HEALTH_MESSAGE})));
}

/**
 * Given the health 1001, one over 1 to 1000
 * When it is read in that range
 * Then a DataFileValueOutOfRangeException is thrown
 */
TEST(DataFileReading, AWholeNumberOverItsRangeIsRefused) {
  const toml::table root = parseDataFile(ONE_HEALTH_TOO_MANY, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THROW(
      static_cast<void>(wholeNumberAt(gameplay, HEALTH_KEY, HEALTH_RANGE)),
      DataFileValueOutOfRangeException);
}

/**
 * Given the health 3.0, a float that is whole
 * When it is read in 1 to 1000
 * Then a DataFileValueOutOfRangeException is thrown: the keys take TOML
 * integers
 */
TEST(DataFileReading, AWholeFloatIsRefused) {
  const toml::table root = parseDataFile(HEALTH_AS_A_FLOAT, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THROW(
      static_cast<void>(wholeNumberAt(gameplay, HEALTH_KEY, HEALTH_RANGE)),
      DataFileValueOutOfRangeException);
}

/**
 * Given a table gameplay whose interval_ms is 1500
 * When it is read as milliseconds, from 1 to 60000
 * Then it is a duration of 1.5 seconds
 */
TEST(DataFileReading, MillisecondsAreReadAsADuration) {
  const toml::table root = parseDataFile(AN_INTERVAL, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_EQ(millisecondsAt(gameplay, INTERVAL_KEY, INTERVAL_RANGE),
            ONE_AND_A_HALF_SECONDS);
}

/**
 * Given a table gameplay whose side is "enemy"
 * When its side is read as a text
 * Then it is enemy
 */
TEST(DataFileReading, ATextIsRead) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_EQ(textAt(gameplay, SIDE_KEY), ENEMY);
}

/**
 * Given a table gameplay whose side is the number 2
 * When its side is read as a text
 * Then a DataFileTextExpectedException names the file, line 2, the column of
 * the value and the key
 */
TEST(DataFileReading, ANumberWhereATextIsExpectedIsRefused) {
  const toml::table root = parseDataFile(SIDE_AS_A_NUMBER, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT([&gameplay] { static_cast<void>(textAt(gameplay, SIDE_KEY)); },
              ThrowsMessage<DataFileTextExpectedException>(
                  StrEq(std::string{SIDE_NOT_A_TEXT_MESSAGE})));
}

/**
 * Given a table gameplay whose hits are the texts "player" and "wall"
 * When its hits are read as texts
 * Then they are player then wall, each with the line and the column it is
 * written at and the name of the key
 */
TEST(DataFileReading, AnArrayOfTextsIsReadInTheOrderOfTheFile) {
  const toml::table root = parseDataFile(TWO_HITS, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  const std::vector<DataFileText> hits = textsAt(gameplay, HITS_KEY);

  ASSERT_EQ(hits.size(), 2U);
  EXPECT_EQ(hits[0].text, PLAYER_TEXT);
  EXPECT_EQ(hits[0].place.line, LINE_OF_THE_HITS);
  EXPECT_EQ(hits[0].place.column, COLUMN_OF_THE_FIRST_HIT);
  EXPECT_EQ(hits[1].text, WALL_TEXT);
  EXPECT_EQ(hits[1].place.column, COLUMN_OF_THE_SECOND_HIT);
  EXPECT_EQ(hits[1].place.key, NAME_OF_THE_HITS);
}

/**
 * Given a table gameplay whose hits are an empty array
 * When its hits are read as texts
 * Then there is none
 */
TEST(DataFileReading, AnEmptyArrayHoldsNoText) {
  const toml::table root = parseDataFile(NO_HIT, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_TRUE(textsAt(gameplay, HITS_KEY).empty());
}

/**
 * Given a table gameplay whose hits are one text, not an array
 * When its hits are read as texts
 * Then a DataFileTextsExpectedException names the file, line 2, the column of
 * the value and the key
 */
TEST(DataFileReading, ASingleTextIsNotAnArrayOfTexts) {
  const toml::table root = parseDataFile(HITS_AS_A_TEXT, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT([&gameplay] { static_cast<void>(textsAt(gameplay, HITS_KEY)); },
              ThrowsMessage<DataFileTextsExpectedException>(
                  StrEq(std::string{HITS_NOT_AN_ARRAY_MESSAGE})));
}

/**
 * Given a table gameplay whose hits hold a text, then the number 3
 * When its hits are read as texts
 * Then a DataFileTextsExpectedException names the column of the number
 */
TEST(DataFileReading, ANumberAmongTheTextsIsRefusedAtTheNumber) {
  const toml::table root =
      parseDataFile(A_NUMBER_AMONG_THE_HITS, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT([&gameplay] { static_cast<void>(textsAt(gameplay, HITS_KEY)); },
              ThrowsMessage<DataFileTextsExpectedException>(
                  StrEq(std::string{NUMBER_AMONG_THE_HITS_MESSAGE})));
}

/**
 * Given a table gameplay with no hits
 * When its hits are read as texts
 * Then a MissingDataFileKeyException is thrown
 */
TEST(DataFileReading, AnAbsentArrayOfTextsIsMissing) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THROW(static_cast<void>(textsAt(gameplay, HITS_KEY)),
               MissingDataFileKeyException);
}

/**
 * Given a table gameplay whose side is "enemy", and the choices player and
 * enemy
 * When its side is read as one of them
 * Then it is the second choice
 */
TEST(DataFileReading, AChoiceIsItsPosition) {
  const toml::table root = parseDataFile(GAMEPLAY_WITH_A_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_EQ(choiceAt(gameplay, SIDE_KEY, TWO_SIDES), POSITION_OF_ENEMY);
}

/**
 * Given a table gameplay whose side is "enmy", and the choices player and
 * enemy
 * When its side is read as one of them
 * Then an UnknownDataFileChoiceException names the place, the key, the text
 * and the choices
 */
TEST(DataFileReading, AnUnknownChoiceListsTheKnownOnes) {
  const toml::table root = parseDataFile(MISSPELT_SIDE, SHEET_FILE_NAME);
  const DataFileTable gameplay =
      tableAt(rootOf(root, SHEET_FILE_NAME), GAMEPLAY_TABLE);

  EXPECT_THAT(
      [&gameplay] {
        static_cast<void>(choiceAt(gameplay, SIDE_KEY, TWO_SIDES));
      },
      ThrowsMessage<UnknownDataFileChoiceException>(
          StrEq(std::string{MISSPELT_SIDE_MESSAGE})));
}

/**
 * Given a folder holding weaver.toml, drifter.toml, notes.md and
 * nested/ship.toml
 * When its data files are listed
 * Then they are drifter.toml then weaver.toml: the files of the folder ending
 * in .toml, by name
 */
TEST(DataFileReading, AFolderListsItsDataFilesByName) {
  const TemporaryFolder folder;
  folder.write(WEAVER_FILE, A_SIDE);
  folder.write(DRIFTER_FILE, A_SIDE);
  folder.write(NOTES_FILE, A_SIDE);
  folder.write(NESTED_SHIP_FILE, A_SIDE);

  EXPECT_EQ(dataFilesIn(folder.path()),
            (std::vector<std::filesystem::path>{folder.path() / DRIFTER_FILE,
                                                folder.path() / WEAVER_FILE}));
}

/**
 * Given a folder that does not exist
 * When its data files are listed
 * Then an UnreadableDataFileException names it by its absolute path
 */
TEST(DataFileReading, AFolderThatDoesNotExistIsNamed) {
  const std::filesystem::path folder{MISSING_FOLDER};
  const std::string expected =
      std::string{UNREADABLE_START} +
      std::filesystem::absolute(folder).generic_string();

  EXPECT_THAT([&folder] { static_cast<void>(dataFilesIn(folder)); },
              ThrowsMessage<UnreadableDataFileException>(StrEq(expected)));
}

/**
 * Given a data file whose line ends in CRLF
 * When its content is read
 * Then it is every byte of the file, the CR included
 */
TEST(DataFileReading, TheContentOfAFileIsReadAsBytes) {
  const TemporaryFolder folder;
  folder.write(CRLF_FILE, CRLF_CONTENT);

  EXPECT_EQ(dataFileContent(folder.path() / CRLF_FILE), CRLF_CONTENT);
}
