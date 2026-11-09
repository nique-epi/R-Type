#include "InstallFixtures.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

namespace rtype::client::testing {

std::filesystem::path emptyFolder(std::string_view name) {
  const std::filesystem::path folder =
      std::filesystem::temp_directory_path() / name;
  std::filesystem::remove_all(folder);
  std::filesystem::create_directories(folder);
  return folder;
}

void writeFile(const std::filesystem::path& file, std::string_view content) {
  std::ofstream{file} << content;
}

std::string contentOf(const std::filesystem::path& file) {
  std::ifstream input{file};
  return {std::istreambuf_iterator<char>{input},
          std::istreambuf_iterator<char>{}};
}

}  // namespace rtype::client::testing
