#pragma once

#include <cstdint>

namespace rtype::logging {

/** @brief How the journal file writes a line: the text a person reads, or one
 * JSON object per line (JSON Lines) for a tool. Standard error always shows
 * the text. */
enum class LogFormat : std::uint8_t {
  Text,
  Json,
};

}  // namespace rtype::logging
