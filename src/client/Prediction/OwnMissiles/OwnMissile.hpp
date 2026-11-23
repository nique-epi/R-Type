#pragma once

#include <cstdint>
#include <optional>
#include "Entity.hpp"
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief A missile the client fired itself: the input that fired it, its rank
 * among the missiles of that shot, where it left the muzzle, how fast it
 * flies on each axis, in logical units per second, the type it is drawn as,
 * the entity that draws it (nothing once it touched an enemy on screen), and
 * the network id of the missile of the server it stands for, once the server
 * said so.
 */
struct OwnMissile {
  std::uint32_t firingInput{};
  std::uint8_t shotRank{0};
  game::Position start{};
  game::Position velocity{};
  std::uint8_t projectileType{0};
  std::optional<engine::Entity> entity;
  std::optional<std::uint32_t> serverId;
};

}  // namespace rtype::client
