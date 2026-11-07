#pragma once

#include <cstdint>
#include "IncomingDatagram.hpp"

namespace rtype::server::testing {

/** @returns The datagram of a probe that carries @p secret and @p token, as
 * the master sends it. */
[[nodiscard]] network::IncomingDatagram probeArriving(std::uint64_t secret,
                                                      std::uint64_t token);

}  // namespace rtype::server::testing
