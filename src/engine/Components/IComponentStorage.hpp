#pragma once

#include <cstdint>

namespace rtype::engine {

/** @brief Type-erased view of a component storage. */
class IComponentStorage {
 public:
  virtual ~IComponentStorage() = default;

  /** @returns true when a component of the entity index was removed. */
  virtual bool erase(std::uint32_t entityIndex) = 0;
};

}  // namespace rtype::engine
