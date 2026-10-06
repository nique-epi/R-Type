#pragma once

#include <filesystem>
#include <string>
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

 private:
  std::filesystem::path path_;
};
