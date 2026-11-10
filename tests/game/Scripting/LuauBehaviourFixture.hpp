#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include "BehaviourRegistry.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "IBehaviourMemory.hpp"
#include "IBodyOrders.hpp"
#include "Intent.hpp"
#include "Position.hpp"

/** @brief A body a test steers over several ticks: where it is, its network
 * id, and what its behaviour remembers of it. */
struct SteeredBody {
  rtype::game::Position position{};
  std::uint32_t networkId{0};
  std::shared_ptr<rtype::game::IBehaviourMemory> memory = nullptr;
};

/** @brief The scripts of a test, steering bodies tick after tick as the
 * pilots of a game do. */
class LuauBehaviourFixture : public ::testing::Test {
 protected:
  LuauBehaviourFixture();

  /** @brief Runs @p script and keeps the behaviours it registers. */
  void load(std::string_view script);
  /** @brief Makes @p orders carry out the bodies the scripts order. */
  void attach(rtype::game::IBodyOrders& orders);
  /** @brief Gives the scripts the budget of a new tick. */
  void beginTick();
  /** @returns What @p behaviour wants for @p body in this tick. */
  [[nodiscard]] rtype::game::Intent steer(std::string_view behaviour,
                                          SteeredBody& body);
  /** @returns The memory the scripts hold once every garbage is collected. */
  [[nodiscard]] std::size_t memoryAfterCollection();
  /** @returns The failures reported so far. */
  [[nodiscard]] const FailureLog& failures() const;

 private:
  FailureLog log_;
  rtype::game::BehaviourScripts scripts_;
  rtype::game::BehaviourRegistry behaviours_;
};
