#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>

namespace rtype::server {

/** @brief Cuts what a child writes into lines, without their end of line or
 * the '\r' before it. */
class ChildOutputLines {
 public:
  /** @returns The lines @p bytes completes, the text kept from the previous
   * calls in front of the first; a line that reaches MAXIMUM_CHILD_LINE_LENGTH
   * bytes is handed at that length, and what follows starts the next. */
  [[nodiscard]] std::vector<std::string> feed(std::span<const char> bytes);

  /** @returns The text after the last end of line, once the child closed its
   * output; nothing when none is left. */
  [[nodiscard]] std::optional<std::string> finish();

 private:
  /** @brief Hands the line being read to @p lines, and starts the next. */
  void endLine(std::vector<std::string>& lines);

  std::string pending_;
};

}  // namespace rtype::server
