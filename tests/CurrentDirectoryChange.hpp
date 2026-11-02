#pragma once

#include <filesystem>

/** @brief Makes a directory the current one until it is destroyed. */
class CurrentDirectoryChange {
 public:
  explicit CurrentDirectoryChange(const std::filesystem::path& directory);
  ~CurrentDirectoryChange();

  CurrentDirectoryChange(const CurrentDirectoryChange&) = delete;
  CurrentDirectoryChange& operator=(const CurrentDirectoryChange&) = delete;
  CurrentDirectoryChange(CurrentDirectoryChange&&) = delete;
  CurrentDirectoryChange& operator=(CurrentDirectoryChange&&) = delete;

 private:
  std::filesystem::path previous_;
};
