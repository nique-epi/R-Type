#pragma once

#include <cstdint>
#include "Position.hpp"
#include "SpriteAppearance.hpp"

namespace rtype::game {

struct Sprite;

}  // namespace rtype::game

namespace rtype::client {

struct Caption;

}  // namespace rtype::client

namespace rtype::client {

/** @brief One sprite or one caption to draw, in its layer. */
struct DrawCall {
  std::uint32_t entityIndex{};
  game::Position position{};
  const game::Sprite* sprite{};
  const Caption* caption{};
  SpriteAppearance look;
};

}  // namespace rtype::client
