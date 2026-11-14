#pragma once

#include <memory>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "LuauBehaviour.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauHitHandler.hpp"
#include "LuauHitHandlerParts.hpp"

namespace rtype::game {

/**
 * @brief What the scripts of a BehaviourScripts declare as they are read,
 * kept until they are handed over with the built-in behaviours: their
 * behaviours (`behaviours.register`) and their hit handlers
 * (`behaviours.on_hit`).
 */
class LuauDeclarations {
 public:
  /** @brief Keeps the behaviour @p parts describe. */
  void behaviour(LuauBehaviourParts parts);

  /** @brief Keeps the hit handler @p parts describe. */
  void hitHandler(LuauHitHandlerParts parts);

  /**
   * @brief Adds every declaration to @p registry, which they leave.
   * @throws DuplicateBehaviourException when a behaviour has a name the
   * registry already holds; DuplicateHitHandlerException when a hit handler
   * does.
   */
  void handOverTo(BehaviourRegistry& registry);

 private:
  std::vector<std::unique_ptr<LuauBehaviour>> behaviours_;
  std::vector<std::unique_ptr<LuauHitHandler>> hitHandlers_;
};

}  // namespace rtype::game
