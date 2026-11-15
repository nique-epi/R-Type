#pragma once

#include <cstddef>
#include "Entity.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/** @brief Gives @p body a Summoning of @p ticks ticks, so it stays harmless
 * until then, and puts the first shot of its Gun, if it has one, off by as
 * long. Never called while the components are visited. */
void startSummoning(engine::ComponentRegistry& components, engine::Entity body,
                    std::size_t ticks);

}  // namespace rtype::game
