#include "JournalFile.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>
#include "Logger.hpp"

JournalFile::JournalFile() {
  const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
  path_ = std::filesystem::temp_directory_path() /
          (std::string("rtype_") + info->test_suite_name() + "_" +
           info->name() + ".log");
  std::error_code ignored;
  std::filesystem::remove(path_, ignored);
}

JournalFile::~JournalFile() {
  rtype::logging::Logger::setOutput(rtype::logging::LogOutput{});
  std::error_code ignored;
  std::filesystem::remove(path_, ignored);
}

const std::filesystem::path& JournalFile::path() const { return path_; }

std::vector<std::string> JournalFile::lines() const {
  std::vector<std::string> lines;
  std::ifstream file(path_);
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  return lines;
}
