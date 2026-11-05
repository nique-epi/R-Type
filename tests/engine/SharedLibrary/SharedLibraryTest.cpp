#include <gtest/gtest.h>
#include <string>
#include "EngineException.hpp"
#include "PluginConstants.hpp"
#include "PluginFiles.hpp"
#include "SharedLibrary.hpp"
#include "SharedLibraryTestConstants.hpp"

using rtype::engine::PLUGIN_ENTRY_NAME;
using rtype::engine::SharedLibrary;
using rtype::engine::SharedLibraryOpenException;

/**
 * Given a folder of plugins with no file called ghost
 * When that file is opened as a shared library
 * Then the open throws and its message names the file
 */
TEST(SharedLibrary, OpeningAMissingFileThrowsAndNamesIt) {
  try {
    const SharedLibrary opened{pluginFile(GHOST_LIBRARY_NAME)};
    ADD_FAILURE() << "the library was opened";
  } catch (const SharedLibraryOpenException& error) {
    EXPECT_NE(std::string{error.what()}.find(GHOST_LIBRARY_NAME),
              std::string::npos);
  }
}

/**
 * Given the probe plugin opened as a shared library
 * When the symbol rtypePluginEntry is looked up
 * Then it is found
 */
TEST(SharedLibrary, TheEntryOfAPluginIsFound) {
  const SharedLibrary library{pluginFile(LIBRARY_WITH_ENTRY_NAME)};

  EXPECT_NE(library.symbol(std::string{PLUGIN_ENTRY_NAME}), nullptr);
}

/**
 * Given the probe plugin opened as a shared library
 * When a symbol it does not export is looked up
 * Then the answer is null
 */
TEST(SharedLibrary, ASymbolTheLibraryDoesNotExportIsNull) {
  const SharedLibrary library{pluginFile(LIBRARY_WITH_ENTRY_NAME)};

  EXPECT_EQ(library.symbol(UNEXPORTED_SYMBOL_NAME), nullptr);
}
