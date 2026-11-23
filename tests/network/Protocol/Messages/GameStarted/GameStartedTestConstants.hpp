#pragma once

namespace rtype::network::testing {

/** @brief Where the biome of the GameStarted tests began: 4096 units before
 * the start of the scroll, as a level opening in it tells, the bytes
 * C5 80 00 00. The biome, `wreck`, the scroll at 240 units and the values no
 * start can have are those of LevelStatusTestConstants.hpp, since the fields
 * follow the rules of LevelStatus. */
constexpr float STARTED_BIOME_SINCE = -4096.0F;

}  // namespace rtype::network::testing
