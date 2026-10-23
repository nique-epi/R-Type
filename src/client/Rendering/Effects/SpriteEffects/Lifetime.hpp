#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Component of an entity that lasts a while, then is destroyed by
 * SpriteEffectSystem: the white picture a ship leaves while it freezes. */
struct Lifetime {
  engine::Duration remaining{};
};

}  // namespace rtype::client
