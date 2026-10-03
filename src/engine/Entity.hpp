#pragma once

#include <cstdint>

namespace rtype::engine {

/**
 * @brief Opaque identifier of a game object.
 */
struct Entity {
  std::uint32_t id;

  bool operator==(const Entity&) const = default;
};

}  // namespace rtype::engine
