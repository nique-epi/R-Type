#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief A temporary journal file named after the running test.
 *
 * The destructor resets the logger to its default output, which closes the
 * journal, before removing the file.
 */
class JournalFile {
 public:
  JournalFile();
  ~JournalFile();

  JournalFile(const JournalFile&) = delete;
  JournalFile& operator=(const JournalFile&) = delete;
  JournalFile(JournalFile&&) = delete;
  JournalFile& operator=(JournalFile&&) = delete;

  [[nodiscard]] const std::filesystem::path& path() const;

  /** @return the journal, one string per line; empty when it does not exist. */
  [[nodiscard]] std::vector<std::string> lines() const;

  /** @return the lines of the journal that hold @p text, oldest first. */
  [[nodiscard]] std::vector<std::string> linesHolding(
      std::string_view text) const;

 private:
  std::filesystem::path path_;
};
