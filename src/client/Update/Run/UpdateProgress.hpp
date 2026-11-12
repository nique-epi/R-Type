#pragma once

#include "UpdatePhase.hpp"

namespace rtype::client {

/** @brief What the update screen shows: the phase, and the share of the
 * package received, from 0 to FULL_DOWNLOAD_PERCENT. */
struct UpdateProgress {
  UpdatePhase phase{UpdatePhase::Downloading};
  int percent{};
};

}  // namespace rtype::client
