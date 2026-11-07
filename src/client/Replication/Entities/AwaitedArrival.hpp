#pragma once

#include "Sprite.hpp"

namespace rtype::client {

/**
 * @brief An entity the server created ahead of the tick the client shows: it
 * holds the sprite back, so nothing draws the entity, and its appearance
 * waits until the tick shown reaches its first state (see
 * revealArrivedEntities()).
 */
struct AwaitedArrival {
  game::Sprite sprite;
};

}  // namespace rtype::client
