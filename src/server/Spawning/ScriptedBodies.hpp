#pragma once

#include <cstdint>
#include <vector>
#include "IScriptWorld.hpp"
#include "Logger.hpp"
#include "ScriptedBodiesParts.hpp"
#include "ScriptedBodyOrder.hpp"
#include "ScriptedSpawn.hpp"

namespace rtype::server {

/**
 * @brief Carries out what the scripts of a game ask of its world.
 *
 * Bodies cannot be created while PilotSystem visits them, so spawn() and
 * destroy() only take note: the body gets its network id at once, so a script
 * can name it, and onTick() makes it appear, tells every connected player, and
 * then removes the bodies asked for, in the same tick, with an
 * EntityDestruction Removed. A body appears before the removals, so a script
 * can remove in a tick the body it created in it.
 *
 * The first tick whose scripts use the whole of their budget is written in the
 * journal, once per world: bodies then wait for the next tick to step.
 *
 * It works on one world, and attaches itself to the scripts for as long as it
 * lives: the game builds a new one with each new world. Not thread-safe: use
 * it from the simulation thread only.
 */
class ScriptedBodies final : public game::IScriptWorld {
 public:
  explicit ScriptedBodies(ScriptedBodiesParts parts);
  ScriptedBodies(const ScriptedBodies&) = delete;
  ScriptedBodies& operator=(const ScriptedBodies&) = delete;
  ScriptedBodies(ScriptedBodies&&) = delete;
  ScriptedBodies& operator=(ScriptedBodies&&) = delete;
  ~ScriptedBodies() override;

  /** @brief Gives the scripts the budget of a new tick. */
  void beginTick();

  /** @brief Carries out what the scripts asked since the last call. */
  void onTick();

  [[nodiscard]] std::uint32_t spawn(const game::ScriptedSpawn& spawn) override;
  void destroy(std::uint32_t networkId) override;

 private:
  void warnWhenOverBudget();
  void appear(const ScriptedBodyOrder& order);
  void removeAsked();

  ScriptedBodiesParts parts_;
  logging::Logger logger_;
  bool budgetWarned_{false};
  std::vector<ScriptedBodyOrder> orders_;
  std::vector<std::uint32_t> removals_;
};

}  // namespace rtype::server
