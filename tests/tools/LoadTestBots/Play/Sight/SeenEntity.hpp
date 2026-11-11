#pragma once

#include <cstdint>
#include "EntityKind.hpp"

namespace rtype::tools {

/** @brief An entity a bot sees, its velocity in logical units per second: 0
 * in the first state that lists it. */
struct SeenEntity {
  std::uint32_t entityId{};
  EntityKind kind{EntityKind::Enemy};
  float x{};
  float y{};
  float velocityX{};
  float velocityY{};
  /** @brief The size of its hitbox, in logical units. */
  float width{};
  float height{};
};

}  // namespace rtype::tools
