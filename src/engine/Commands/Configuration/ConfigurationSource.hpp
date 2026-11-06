#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "HandOverRule.hpp"

namespace rtype::engine {

/**
 * @brief What the configuration of a program is read from, and which of its
 * lines belong to another program.
 */
struct ConfigurationSource {
  /** @brief The file of commands, one per line; empty to read no file. */
  std::filesystem::path file;
  /** @brief Lines run after the file, built from the launch options. */
  std::vector<std::string> forcedLines;
  /** @brief Which lines, of the file or forced, are kept for another program
   * instead of being run; empty to keep none. */
  HandOverRule handOver;
};

}  // namespace rtype::engine
