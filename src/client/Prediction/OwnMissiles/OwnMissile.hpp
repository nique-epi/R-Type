#pragma once

#include <cstdint>
#include <optional>
#include "Entity.hpp"
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief A missile the client fired itself: the input that fired it, where it
 * left the muzzle, the entity that draws it (nothing once it touched an enemy
 * on screen), and the network id of the missile of the server it stands for,
 * once the server said so.
 */
struct OwnMissile {
  std::uint32_t firingInput{};
  game::Position start{};
  std::optional<engine::Entity> entity;
  std::optional<std::uint32_t> serverId;
};

}  // namespace rtype::client
