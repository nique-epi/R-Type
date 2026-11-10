#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <string_view>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "EventBus.hpp"
#include "Intent.hpp"
#include "LuauBehaviourFixture.hpp"
#include "ScriptEvent.hpp"

/** @brief The scripts of a test with the bus of a game: bodies are steered
 * tick after tick, the test publishes and dispatches as the game loop does,
 * and every event a script publishes is recorded, before any script hears
 * it. */
class LuauEventBindingsFixture : public ::testing::Test {
 protected:
  LuauEventBindingsFixture();

  /** @brief Runs @p script and keeps the behaviours it registers. */
  void load(std::string_view script);
  /** @returns What @p behaviour wants for @p body in a new tick. */
  rtype::game::Intent steer(std::string_view behaviour, SteeredBody& body);
  /** @returns The bus of the game. */
  [[nodiscard]] rtype::engine::EventBus& bus();
  /** @returns Whether the scripts have budget left in this tick. */
  [[nodiscard]] bool scriptsHaveBudget();
  /** @returns The events the scripts published and the bus delivered. */
  [[nodiscard]] const std::vector<rtype::game::ScriptEvent>& published() const;
  /** @returns The event `relay` publishes for the body of @p networkId. */
  [[nodiscard]] static rtype::game::ScriptEvent heardBy(
      std::uint32_t networkId);
  /** @returns The failures reported so far. */
  [[nodiscard]] const FailureLog& failures() const;

 private:
  FailureLog log_;
  rtype::game::BehaviourScripts scripts_;
  rtype::game::BehaviourRegistry behaviours_;
  rtype::engine::EventBus bus_;
  std::vector<rtype::game::ScriptEvent> published_;
};
