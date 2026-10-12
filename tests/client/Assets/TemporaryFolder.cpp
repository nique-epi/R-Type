#include "TemporaryFolder.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <string_view>
#include <system_error>

namespace {

std::string currentTestName() {
  const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
  std::string name =
      std::string("rtype_") + info->test_suite_name() + "_" + info->name();
  std::ranges::replace(name, '/', '_');
  return name;
}

}  // namespace

TemporaryFolder::TemporaryFolder()
    : path_(std::filesystem::temp_directory_path() / currentTestName()) {
  std::filesystem::remove_all(path_);
  std::filesystem::create_directories(path_);
}

TemporaryFolder::~TemporaryFolder() {
  std::error_code ignored;
  std::filesystem::remove_all(path_, ignored);
}

const std::filesystem::path& TemporaryFolder::path() const { return path_; }

void TemporaryFolder::write(const std::filesystem::path& relativeFile,
                            std::string_view content) const {
  const std::filesystem::path file = path_ / relativeFile;
  std::filesystem::create_directories(file.parent_path());
  std::ofstream stream(file, std::ios::binary);
  stream.write(content.data(), static_cast<std::streamsize>(content.size()));
}
