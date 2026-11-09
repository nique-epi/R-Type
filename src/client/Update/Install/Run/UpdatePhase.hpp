#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where an update stands: its package downloading, then installing,
 * then ready to start the new version; or failed, at any step. */
enum class UpdatePhase : std::uint8_t {
  Downloading,
  Installing,
  ReadyToRestart,
  Failed,
};

}  // namespace rtype::client
