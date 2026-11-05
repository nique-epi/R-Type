#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

namespace rtype::network {

/**
 * @brief A file of rows separated by commas, open for as long as the object
 * lives: the traffic file of a program, or the file of the game statistics
 * plugin of the server.
 */
class CsvFile {
 public:
  /**
   * @brief Creates @p path, or empties it, and writes @p header as its first
   * line: a file is one run of the program.
   * @throws CsvFileOpenException when the system refuses to open it.
   */
  CsvFile(const std::filesystem::path& path, std::string_view header);

  /**
   * @brief Appends @p rows and flushes them to the system, so they stay even
   * if the program crashes.
   * @returns Whether everything written so far reached the system.
   */
  [[nodiscard]] bool append(std::string_view rows);

 private:
  std::ofstream file_;
};

}  // namespace rtype::network
