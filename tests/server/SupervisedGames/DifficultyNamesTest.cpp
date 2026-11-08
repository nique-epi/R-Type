#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include "DifficultyNames.hpp"
#include "DifficultyNamesTestConstants.hpp"
#include "GameLaunchSettings.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::server::difficultyNamesOf;
using rtype::server::GameLaunchSettings;

/**
 * Given the shipped assets and the example resource, whose sheet names a
 * behaviour its server script registers
 * When the difficulties of the waves of the resource, laid over those of the
 * assets, are read
 * Then they are the tables of those waves files
 */
TEST(DifficultyNames, ReadsTheWavesOfAResourceWhoseSheetIsScripted) {
  const std::filesystem::path assets{RTYPE_SOURCE_ASSETS_FOLDER};
  const std::filesystem::path resource =
      assets / EXAMPLE_RESOURCE_FROM_THE_ASSETS;
  GameLaunchSettings settings{};
  settings.wavesFiles = {assets / WAVES_OF_THE_EXAMPLE_RESOURCE,
                         resource / WAVES_OF_THE_EXAMPLE_RESOURCE};
  settings.contentFolders = {assets, resource};

  const std::vector<std::string> names = difficultyNamesOf(settings);

  EXPECT_EQ(names,
            (std::vector<std::string>{EXAMPLE_RESOURCE_DIFFICULTIES.begin(),
                                      EXAMPLE_RESOURCE_DIFFICULTIES.end()}));
}
