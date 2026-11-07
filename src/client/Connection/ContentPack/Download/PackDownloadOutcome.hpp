#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where a download of a content pack stands. */
enum class PackDownloadOutcome : std::uint8_t {
  /** @brief Bytes are still coming. */
  Running,
  /** @brief Every byte the server announced came. */
  Finished,
  /** @brief The server could not be reached, refused, sent more than it
   * announced, or each try was cut. */
  Failed,
};

}  // namespace rtype::client
