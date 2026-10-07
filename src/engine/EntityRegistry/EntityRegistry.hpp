#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Entity.hpp"

namespace rtype::engine {

/**
 * @brief Hands out entities and recycles the index of destroyed ones. It knows
 * neither rendering nor networking.
 */
class EntityRegistry {
 public:
  /**
   * @brief Creates an entity, reusing the index of a destroyed one when one is
   * available.
   */
  Entity create();

  /**
   * @brief Destroys an entity.
   *
   * Does nothing when the entity is not alive: destroying twice, or through a
   * handle whose index was recycled, never affects another entity.
   */
  void destroy(Entity entity);

  /**
   * @returns true only for an entity returned by create() and not destroyed
   * since.
   */
  bool isAlive(Entity entity) const;

  /**
   * @returns The entity currently using the slot of entityIndex.
   * The slot must hold an alive entity, for instance because a component is
   * attached to that index: a free slot is not told apart from an alive one.
   */
  Entity entityAt(std::uint32_t entityIndex) const;

  /**
   * @returns The number of alive entities.
   */
  std::size_t size() const;

 private:
  std::vector<std::uint32_t> generations_;
  std::vector<std::uint32_t> freeIndices_;
};

}  // namespace rtype::engine
