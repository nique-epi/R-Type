#pragma once

#include <string_view>

namespace rtype::server {

/** @brief Where the commands of the console write what they answer. */
class IConsoleOutput {
 public:
  virtual ~IConsoleOutput() = default;

  /** @brief Writes @p line, then an end of line, for the administrator to
   * read. Called from the simulation thread, and from the thread that reads
   * the configuration before the loop starts. */
  virtual void write(std::string_view line) = 0;
};

}  // namespace rtype::server
