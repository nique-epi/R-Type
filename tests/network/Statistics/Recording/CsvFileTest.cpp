#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "CsvFile.hpp"
#include "CsvFileTestConstants.hpp"
#include "NetworkException.hpp"
#include "NonAnsiFolderTestConstants.hpp"
#include "TemporaryFolder.hpp"
#include "TrafficFixtures.hpp"

using rtype::network::CsvFile;
using rtype::network::CsvFileOpenException;
using rtype::network::testing::textOf;

/**
 * Given a file that held an earlier run
 * When a CSV file opens it with a header
 * Then it holds the header and nothing else
 */
TEST(CsvFile, StartsAFreshRunWithItsHeader) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / CSV_FILE_NAME;
  folder.write(CSV_FILE_NAME, EARLIER_RUN_TEXT);

  const CsvFile opened{file, CSV_HEADER};

  EXPECT_EQ(textOf(file), CSV_HEADER_LINE);
}

/**
 * Given an open CSV file
 * When a row is appended
 * Then another reader finds it at once, without the file being closed
 */
TEST(CsvFile, ARowAppendedReachesTheFileAtOnce) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / CSV_FILE_NAME;
  CsvFile opened{file, CSV_HEADER};

  EXPECT_TRUE(opened.append(SOME_ROW));

  EXPECT_EQ(textOf(file), std::string{CSV_HEADER_LINE} + std::string{SOME_ROW});
}

/**
 * Given a path that is a folder whose name is outside the code page
 * When a CSV file opens it
 * Then it refuses, and the message names the folder in UTF-8
 */
TEST(CsvFile, NamesAFolderOutsideTheCodePageInUtf8) {
  const TemporaryFolder folder;
  const std::filesystem::path nonAnsiFolder =
      folder.path() / rtype::testing::NON_ANSI_FOLDER_NAME;
  std::filesystem::create_directories(nonAnsiFolder);

  try {
    const CsvFile opened{nonAnsiFolder, CSV_HEADER};
    ADD_FAILURE() << "a folder was opened as a CSV file";
  } catch (const CsvFileOpenException& error) {
    EXPECT_NE(std::string{error.what()}.find(
                  std::string{rtype::testing::NON_ANSI_FOLDER_BYTES}),
              std::string::npos);
  }
}

/**
 * Given a path that is a folder
 * When a CSV file opens it
 * Then it refuses, naming the error of this library
 */
TEST(CsvFile, RefusesAPathTheSystemCannotOpen) {
  const TemporaryFolder folder;

  EXPECT_THROW(const CsvFile opened(folder.path(), CSV_HEADER),
               CsvFileOpenException);
}
