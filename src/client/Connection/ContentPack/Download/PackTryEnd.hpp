#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How one try of a download of a content pack ended. */
enum class PackTryEnd : std::uint8_t {
  /** @brief Every byte of the pack came. */
  Whole,
  /** @brief The try was cut before the end: the next try resumes it. */
  Cut,
  /** @brief The server answered another status or sent more than it
   * announced: no try is made again. */
  Refused,
};

}  // namespace rtype::client
