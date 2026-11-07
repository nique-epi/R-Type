#pragma once

#include <cstdint>
#include "Endpoint.hpp"

namespace rtype::master {

/** @brief Where the probes of the master go for a registered server, and the
 * secret they carry. */
struct ProbeTarget {
  /** @brief The address it registered with, or the one its registration came
   * from, and its UDP port. */
  network::Endpoint endpoint;
  std::uint64_t secret{};
};

}  // namespace rtype::master
