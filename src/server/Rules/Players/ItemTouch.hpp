#pragma once

#include "Entity.hpp"

namespace rtype::server {

/** @brief A ship that touched an item of the world, a look item or a
 * crystal, as CollisionRules reports it. */
struct ItemTouch {
  engine::Entity ship{};
  engine::Entity item{};
};

}  // namespace rtype::server
