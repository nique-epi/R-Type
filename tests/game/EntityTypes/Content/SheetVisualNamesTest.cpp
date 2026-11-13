#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "GameException.hpp"
#include "SheetVisualNames.hpp"
#include "SheetVisualNamesFixtures.hpp"
#include "SheetVisualNamesTestConstants.hpp"

using rtype::engine::DataFileTableExpectedException;
using rtype::game::InconsistentEntitySheetException;
using rtype::game::lookNumberOf;
using rtype::game::namesEffect;
using rtype::game::SheetVisualNames;
using rtype::game::testing::BASE_LOOK;
using rtype::game::testing::BASE_LOOK_NUMBER;
using rtype::game::testing::FIRST_EFFECT;
using rtype::game::testing::FIRST_LOOK;
using rtype::game::testing::FIRST_LOOK_NUMBER;
using rtype::game::testing::LONGEST_LOOK;
using rtype::game::testing::MOST_EFFECTS_OF_A_SHEET;
using rtype::game::testing::SECOND_EFFECT;
using rtype::game::testing::SECOND_LOOK;
using rtype::game::testing::SECOND_LOOK_NUMBER;
using rtype::game::testing::SHEET_WITH_A_CAPITAL_IN_AN_EFFECT;
using rtype::game::testing::SHEET_WITH_A_TOO_LONG_LOOK;
using rtype::game::testing::SHEET_WITH_AN_EFFECT_THAT_IS_NO_TABLE;
using rtype::game::testing::SHEET_WITH_ONE_EFFECT_TOO_MANY;
using rtype::game::testing::SHEET_WITH_THE_LONGEST_LOOK;
using rtype::game::testing::SHEET_WITH_THE_MOST_EFFECTS;
using rtype::game::testing::SHEET_WITH_VISUALS;
using rtype::game::testing::SHEET_WITHOUT_VISUALS;
using rtype::game::testing::TWO_NAMES;
using rtype::game::testing::UNKNOWN_NAME;
using rtype::game::testing::visualNamesIn;

/**
 * Given a sheet whose appearance names no effect and no look
 * When its visual names are read
 * Then both lists are empty
 */
TEST(SheetVisualNames, ASheetWithoutTablesNamesNothing) {
  const SheetVisualNames names = visualNamesIn(SHEET_WITHOUT_VISUALS);

  EXPECT_TRUE(names.effects.empty());
  EXPECT_TRUE(names.looks.empty());
}

/**
 * Given a sheet naming two effects and two looks out of the order of their
 * names
 * When its visual names are read
 * Then each list is in the order of the names
 */
TEST(SheetVisualNames, ListsTheNamesInTheirOrder) {
  const SheetVisualNames names = visualNamesIn(SHEET_WITH_VISUALS);

  EXPECT_EQ(names.effects,
            (std::vector<std::string>{std::string{FIRST_EFFECT},
                                      std::string{SECOND_EFFECT}}));
  EXPECT_EQ(names.looks, (std::vector<std::string>{std::string{FIRST_LOOK},
                                                   std::string{SECOND_LOOK}}));
  EXPECT_EQ(names.looks.size(), TWO_NAMES);
}

/**
 * Given the names of a sheet
 * When an effect is looked up
 * Then each of its effects is found, and no other name
 */
TEST(SheetVisualNames, FindsTheEffectsOfTheSheetOnly) {
  const SheetVisualNames names = visualNamesIn(SHEET_WITH_VISUALS);

  EXPECT_TRUE(namesEffect(names, FIRST_EFFECT));
  EXPECT_TRUE(namesEffect(names, SECOND_EFFECT));
  EXPECT_FALSE(namesEffect(names, UNKNOWN_NAME));
  EXPECT_FALSE(namesEffect(names, FIRST_LOOK));
}

/**
 * Given the names of a sheet
 * When its looks are numbered
 * Then each takes its place in the order of the names from 1, the empty name
 * takes 0, and any other name none
 */
TEST(SheetVisualNames, NumbersTheLooksFromOne) {
  const SheetVisualNames names = visualNamesIn(SHEET_WITH_VISUALS);

  EXPECT_EQ(lookNumberOf(names, BASE_LOOK), std::optional{BASE_LOOK_NUMBER});
  EXPECT_EQ(lookNumberOf(names, FIRST_LOOK), std::optional{FIRST_LOOK_NUMBER});
  EXPECT_EQ(lookNumberOf(names, SECOND_LOOK),
            std::optional{SECOND_LOOK_NUMBER});
  EXPECT_EQ(lookNumberOf(names, UNKNOWN_NAME), std::nullopt);
  EXPECT_EQ(lookNumberOf(names, FIRST_EFFECT), std::nullopt);
}

/**
 * Given a sheet naming the most effects a sheet may name
 * When its visual names are read
 * Then every effect is listed
 */
TEST(SheetVisualNames, ReadsTheMostEffectsASheetMayName) {
  const SheetVisualNames names = visualNamesIn(SHEET_WITH_THE_MOST_EFFECTS);

  EXPECT_EQ(names.effects.size(), MOST_EFFECTS_OF_A_SHEET);
}

/**
 * Given a sheet naming one effect more than a sheet may name
 * When its visual names are read
 * Then the sheet is refused
 */
TEST(SheetVisualNames, RefusesOneEffectTooMany) {
  EXPECT_THROW((void)visualNamesIn(SHEET_WITH_ONE_EFFECT_TOO_MANY),
               InconsistentEntitySheetException);
}

/**
 * Given a look named with 24 letters, the longest name
 * When the visual names are read
 * Then the look is listed
 */
TEST(SheetVisualNames, ReadsTheLongestName) {
  const SheetVisualNames names = visualNamesIn(SHEET_WITH_THE_LONGEST_LOOK);

  EXPECT_EQ(names.looks, (std::vector<std::string>{std::string{LONGEST_LOOK}}));
}

/**
 * Given a look named with 25 letters, and an effect named with a capital
 * letter
 * When the visual names are read
 * Then each sheet is refused
 */
TEST(SheetVisualNames, RefusesANameThatIsNoNameOfTheContent) {
  EXPECT_THROW((void)visualNamesIn(SHEET_WITH_A_TOO_LONG_LOOK),
               InconsistentEntitySheetException);
  EXPECT_THROW((void)visualNamesIn(SHEET_WITH_A_CAPITAL_IN_AN_EFFECT),
               InconsistentEntitySheetException);
}

/**
 * Given an effect that is a number instead of a table
 * When the visual names are read
 * Then the sheet is refused
 */
TEST(SheetVisualNames, RefusesAnEffectThatIsNoTable) {
  EXPECT_THROW((void)visualNamesIn(SHEET_WITH_AN_EFFECT_THAT_IS_NO_TABLE),
               DataFileTableExpectedException);
}
