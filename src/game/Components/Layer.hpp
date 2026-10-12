#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Drawing layers, from the first drawn (behind) to the last drawn (in
 * front).
 *
 * Interface must stay the last value: the client sizes its per-layer storage
 * from it.
 */
enum class Layer : std::uint8_t { Background, Entities, Effects, Interface };

}  // namespace rtype::game
