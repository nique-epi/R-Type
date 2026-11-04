#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

namespace rtype::network {

/** @brief A traffic file, open for as long as the object lives. */
class TrafficCsvFile {
 public:
  /**
   * @brief Creates @p path, or empties it, and writes TRAFFIC_CSV_HEADER: a
   * file is one run of the program.
   * @throws TrafficFileOpenException when the system refuses to open it.
   */
  explicit TrafficCsvFile(const std::filesystem::path& path);

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
