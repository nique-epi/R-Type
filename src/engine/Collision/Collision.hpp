#pragma once

#include "Entity.hpp"

namespace rtype::engine {

/**
 * @brief Event published when the bounds of two entities overlap.
 *
 * The order of first and second carries no meaning: the pair (a, b) and the
 * pair (b, a) describe the same contact, and a subscriber must accept both.
 */
struct Collision {
  Entity first;
  Entity second;

  bool operator==(const Collision&) const = default;
};

}  // namespace rtype::engine
