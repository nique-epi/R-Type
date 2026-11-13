#pragma once

#include <cstdint>

namespace rtype::tools {

/** @brief What a bot tells apart among what it sees. */
enum class EntityKind : std::uint8_t {
  Enemy,
  EnemyMissile,
  Item,
};

}  // namespace rtype::tools
