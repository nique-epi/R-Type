#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>
#include "BehaviourRegistry.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "IBehaviourState.hpp"
#include "Intent.hpp"
#include "RecordingEntityOrders.hpp"

/** @brief The scripts of a test, steering bodies that live in components of
 * their own, as the pilots of a game do. */
class LuauScriptedBodies {
 public:
  LuauScriptedBodies();

  /** @brief Runs @p script and keeps the behaviours it registers. */
  void load(std::string_view script);
  /** @returns A new body, with no component. */
  [[nodiscard]] rtype::engine::Entity addBody();
  /** @returns The components the bodies live in. */
  [[nodiscard]] rtype::engine::ComponentRegistry& components();
  /** @returns What @p behaviour wants for @p body in a new tick. */
  [[nodiscard]] rtype::game::Intent steer(std::string_view behaviour,
                                          rtype::engine::Entity body);
  /** @brief Ends what the behaviours keep of @p body, as its destruction
   * does. */
  void endBody(rtype::engine::Entity body);
  /** @returns The failures reported so far. */
  [[nodiscard]] const FailureLog& failures() const;

 private:
  FailureLog log_;
  rtype::game::BehaviourScripts scripts_;
  rtype::game::BehaviourRegistry behaviours_;
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  RecordingEntityOrders orders_;
  std::unordered_map<std::uint32_t,
                     std::shared_ptr<rtype::game::IBehaviourState>>
      behaviourStates_;
};
