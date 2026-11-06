#pragma once

#include <filesystem>
#include <span>
#include <string>
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

  /** @brief Writes @p lines to a file below the folder, each ended by a
   * newline, as write() does. */
  void writeLines(const std::filesystem::path& relativeFile,
                  std::span<const std::string> lines) const;

 private:
  std::filesystem::path path_;
};
