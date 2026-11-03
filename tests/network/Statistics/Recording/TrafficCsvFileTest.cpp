#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "NetworkException.hpp"
#include "TemporaryFolder.hpp"
#include "TrafficCsvFile.hpp"
#include "TrafficFileTestConstants.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficTestConstants.hpp"

using rtype::network::TrafficCsvFile;
using rtype::network::TrafficFileOpenException;
using rtype::network::testing::textOf;

/**
 * Given a file that held an earlier run
 * When a traffic file opens it
 * Then it holds the names of the columns and nothing else
 */
TEST(TrafficCsvFile, StartsAFreshRunWithTheNamesOfTheColumns) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TRAFFIC_FILE_NAME;
  folder.write(TRAFFIC_FILE_NAME, "an earlier run\n");

  const TrafficCsvFile opened{file};

  EXPECT_EQ(textOf(file), HEADER_LINE);
}

/**
 * Given an open traffic file
 * When rows are appended
 * Then another reader finds them at once, without the file being closed
 */
TEST(TrafficCsvFile, ARowAppendedReachesTheFileAtOnce) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TRAFFIC_FILE_NAME;
  TrafficCsvFile opened{file};

  EXPECT_TRUE(opened.append("a row\n"));

  EXPECT_EQ(textOf(file), std::string{HEADER_LINE} + "a row\n");
}

/**
 * Given a path that is a folder
 * When a traffic file opens it
 * Then it refuses, naming the error of this library
 */
TEST(TrafficCsvFile, RefusesAPathTheSystemCannotOpen) {
  const TemporaryFolder folder;

  EXPECT_THROW(const TrafficCsvFile opened{folder.path()},
               TrafficFileOpenException);
}
