#include <gtest/gtest.h>
#include <array>
#include <vector>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScriptFilesTestConstants.hpp"
#include "ScriptSource.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::ScriptSource;
using rtype::game::scriptSourcesIn;

/**
 * Given a content folder with two scripts and another file in its server
 * folder, and a content folder without a server folder
 * When the scripts of both are listed
 * Then the two scripts come in the order of their names, named after their
 * folder and their file, with their text
 */
TEST(BehaviourScriptFiles, ListsTheScriptsOfEachFolderInTheOrderOfTheirNames) {
  const TemporaryFolder root;
  root.write(LATER_SCRIPT, LATER_SCRIPT_TEXT);
  root.write(EARLIER_SCRIPT, EARLIER_SCRIPT_TEXT);
  root.write(NOT_A_SCRIPT, LATER_SCRIPT_TEXT);
  const std::array folders{root.path() / FIRST_CONTENT_FOLDER,
                           root.path() / SECOND_CONTENT_FOLDER};

  const std::vector<ScriptSource> sources = scriptSourcesIn(folders);

  ASSERT_EQ(sources.size(), SCRIPTS_OF_THE_FOLDERS);
  EXPECT_EQ(sources[0].name, EARLIER_SCRIPT);
  EXPECT_EQ(sources[0].text, EARLIER_SCRIPT_TEXT);
  EXPECT_EQ(sources[1].name, LATER_SCRIPT);
}
