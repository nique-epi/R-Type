#pragma once

#include "Entity.hpp"
#include "LuauHandleSlot.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/** @brief The body whose script runs a step: the slot of its handle (see
 * LuauEntityHandle::slot()), null between two steps, and its entity in the
 * components of the game the scripts are attached to, null outside a game. */
struct LuauSteppedBody {
  const LuauHandleSlot* handle{nullptr};
  engine::ComponentRegistry* components{nullptr};
  engine::Entity entity{};
};

}  // namespace rtype::game
