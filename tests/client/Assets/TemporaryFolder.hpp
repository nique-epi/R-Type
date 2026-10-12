#pragma once

#include <filesystem>
#include <string_view>

/** @brief Empty folder named after the running test, removed with its content
 * when the object is destroyed. */
class TemporaryFolder {
 public:
  TemporaryFolder();
  ~TemporaryFolder();

  TemporaryFolder(const TemporaryFolder&) = delete;
  TemporaryFolder& operator=(const TemporaryFolder&) = delete;
  TemporaryFolder(TemporaryFolder&&) = delete;
  TemporaryFolder& operator=(TemporaryFolder&&) = delete;

  [[nodiscard]] const std::filesystem::path& path() const;

  /** @brief Writes a file below the folder, creating the folders on the way. */
  void write(const std::filesystem::path& relativeFile,
             std::string_view content) const;

 private:
  std::filesystem::path path_;
};
