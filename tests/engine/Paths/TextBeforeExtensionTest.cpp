#include <gtest/gtest.h>
#include <filesystem>
#include "TextBeforeExtension.hpp"
#include "TextBeforeExtensionTestConstants.hpp"

using rtype::engine::withTextBeforeExtension;

/**
 * Given the file logs/traffic.csv
 * When _game_3 is written before its extension
 * Then the file is logs/traffic_game_3.csv, in the same folder
 */
TEST(TextBeforeExtension, WritesTheTextBetweenTheNameAndTheExtension) {
  EXPECT_EQ(withTextBeforeExtension(TRAFFIC_FILE, GAME_THREE_TEXT),
            std::filesystem::path{TRAFFIC_FILE_OF_GAME_THREE});
}

/**
 * Given the file logs/traffic, which has no extension
 * When _game_3 is written before its extension
 * Then the text ends the name
 */
TEST(TextBeforeExtension, EndsANameWithoutExtension) {
  EXPECT_EQ(withTextBeforeExtension(FILE_WITHOUT_EXTENSION, GAME_THREE_TEXT),
            std::filesystem::path{FILE_WITHOUT_EXTENSION_OF_GAME_THREE});
}
