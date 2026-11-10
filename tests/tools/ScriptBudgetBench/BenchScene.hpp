#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string>
#include "BehaviourScripts.hpp"
#include "BenchPopulation.hpp"
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "SubscriptionHandle.hpp"
#include "SystemClock.hpp"
#include "SystemScheduler.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"
#include "TimedBehaviour.hpp"
#include "World.hpp"

namespace rtype::tools {

/**
 * @brief The world the bench measures: bodies steered by the script of the
 * Crimson Drifter, bullets flown by the built-in behaviour, and four ships.
 *
 * Each tick runs the systems that move the bodies, in the order of a game
 * (PilotSystem, MovementSystem, PlayfieldExitSystem), and delivers their
 * events; a body that left the playfield is replaced on the right edge, so the
 * population is the same at every tick. Nothing is random, so two runs do the
 * same work. Each tick also runs CollisionSystem after them, whose pairs
 * nobody answers; weapons and the sending of the world state are not part of
 * a tick here. Neither copied nor moved: the scripts and the bus report
 * through its address.
 */
class BenchScene {
 public:
  /**
   * @param cellSide The side of a cell of the collision grid.
   * @param contentFolders The folders holding the sheets and the scripts.
   * @throws What BehaviourScripts::run(), EntitySheets::fromTexts() and
   * EntitySheets::named() throw; std::bad_optional_access when the sheet of
   * the scripted bodies names no pilot; engine::InvalidGridLayoutException
   * when the grid refuses @p cellSide.
   */
  BenchScene(BenchPopulation population, float cellSide,
             std::span<const std::filesystem::path> contentFolders);
  BenchScene(const BenchScene&) = delete;
  BenchScene& operator=(const BenchScene&) = delete;
  BenchScene(BenchScene&&) = delete;
  BenchScene& operator=(BenchScene&&) = delete;
  ~BenchScene() = default;

  /** @returns What one tick of SIMULATION_TICK_DURATION took to compute. */
  [[nodiscard]] TickSample runTick();

  /** @returns The bodies in play: the population and the ships. */
  [[nodiscard]] std::size_t bodiesInPlay() const;

  /** @returns The memory the scripts hold, in bytes. */
  [[nodiscard]] std::size_t scriptMemory();

  /** @returns The first failure of the script, empty while it has none: a
   * script that fails stops early, so the figures of that run are wrong. */
  [[nodiscard]] const std::string& scriptFailure() const;

 private:
  void placeShips();
  void fill();
  void replaceWhatLeft();
  void spawnScripted(float across);
  void spawnBullet(float across);
  void timeScriptedBodies();

  BenchPopulation population_;
  std::string scriptFailure_;
  game::BehaviourScripts scripts_;
  game::EntitySheets sheets_;
  engine::Duration scriptTime_{};
  TimedBehaviour timedScript_;
  game::World world_;
  engine::EventBus events_;
  engine::SystemClock clock_;
  engine::SystemScheduler systems_;
  std::size_t scriptedSpawned_{0};
  std::size_t bulletsSpawned_{0};
  std::size_t collisionsFound_{0};
  engine::SubscriptionHandle collisionCounting_;
};

}  // namespace rtype::tools
