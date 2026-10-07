#pragma once

#include <cstdint>

namespace rtype::engine {

/**
 * @brief Reserved network identifier: no entity ever has it, so it can stand
 * for "no entity" on the wire.
 */
constexpr std::uint32_t NO_NETWORK_ID = 0;

}  // namespace rtype::engine
