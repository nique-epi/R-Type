#pragma once

#include <filesystem>

namespace rtype::server {

/**
 * @brief The SQLite file the games of this supervisor keep their results in,
 * for the master module, which sends the vanilla ones to the master.
 */
class IResultsFile {
 public:
  virtual ~IResultsFile() = default;

  /**
   * @returns The file every game of this supervisor is started with: final
   * once the configuration was read, that is when the modules start.
   */
  [[nodiscard]] virtual std::filesystem::path resultsFile() const = 0;
};

}  // namespace rtype::server
