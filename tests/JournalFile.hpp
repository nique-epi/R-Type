#pragma once

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
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

  /** @brief A journal named after the running test and @p suffix, so a test
   * can hold two journals at once. */
  explicit JournalFile(std::string_view suffix);
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

  /** @return the lines of a journal written as JSON Lines whose member
   * "event" is @p name, each read as a JSON object, oldest first. */
  [[nodiscard]] std::vector<nlohmann::json> events(std::string_view name) const;

 private:
  std::filesystem::path path_;
};
