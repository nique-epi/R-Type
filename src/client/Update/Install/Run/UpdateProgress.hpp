#pragma once

#include <cstddef>
#include "UpdatePhase.hpp"

namespace rtype::client {

/** @brief What the update screen shows: the phase, and the bytes of the
 * package received out of its size. */
struct UpdateProgress {
  UpdatePhase phase{UpdatePhase::Downloading};
  std::size_t received{};
  std::size_t size{};
};

}  // namespace rtype::client
