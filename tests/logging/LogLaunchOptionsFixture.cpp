#include "LogLaunchOptionsFixture.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include "LogLaunchOptionsTestConstants.hpp"
#include "Logger.hpp"

using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;
using rtype::logging::testing::MISSING_JOURNAL_FILE;
using rtype::logging::testing::MISSING_JOURNAL_FOLDER;

void LogLaunchOptionsFixture::SetUp() {
  savedLevel_ = Logger::level();
  Logger::setLevel(LogLevel::Warn);
  Logger::setOutput(LogOutput{});
  originalBuffer_ = std::cerr.rdbuf(standardError_.rdbuf());
}

void LogLaunchOptionsFixture::TearDown() {
  std::cerr.rdbuf(originalBuffer_);
  Logger::setLevel(savedLevel_);
  Logger::setOutput(LogOutput{});
}

std::string LogLaunchOptionsFixture::standardError() const {
  return standardError_.str();
}

std::filesystem::path LogLaunchOptionsFixture::missingJournalPath() {
  return std::filesystem::temp_directory_path() / MISSING_JOURNAL_FOLDER /
         MISSING_JOURNAL_FILE;
}
