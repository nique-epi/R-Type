#pragma once

#include <cstddef>
#include <cstdint>
#include "Entity.hpp"

namespace rtype::engine {

/**
 * @brief Hands out entities. It knows neither rendering nor networking.
 */
class EntityRegistry {
 public:
  Entity create();
  std::size_t size() const;

 private:
  std::uint32_t nextId_ = 0;
};

}  // namespace rtype::engine
