#pragma once

#include <cstddef>

namespace rtype::server {

/**
 * @brief What a game shows of its world, whatever the rules of the game: the
 * module that owns the world provides it as a service, and the GameInstance
 * answers through it.
 */
class IGameWorld {
 public:
  virtual ~IGameWorld() = default;

  /** @returns How many entities the world holds now. */
  [[nodiscard]] virtual std::size_t entityCount() const = 0;
};

}  // namespace rtype::server
