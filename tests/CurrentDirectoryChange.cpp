#include "CurrentDirectoryChange.hpp"
#include <filesystem>
#include <system_error>

CurrentDirectoryChange::CurrentDirectoryChange(
    const std::filesystem::path& directory)
    : previous_(std::filesystem::current_path()) {
  std::filesystem::current_path(directory);
}

CurrentDirectoryChange::~CurrentDirectoryChange() {
  std::error_code error;
  std::filesystem::current_path(previous_, error);
}
