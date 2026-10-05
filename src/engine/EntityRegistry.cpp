#include "EntityRegistry.hpp"
#include <cstddef>
#include "Entity.hpp"

namespace rtype::engine {

Entity EntityRegistry::create() { return Entity{nextId_++}; }

std::size_t EntityRegistry::size() const { return nextId_; }

}  // namespace rtype::engine
