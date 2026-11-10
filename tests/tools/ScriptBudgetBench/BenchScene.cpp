#include "BenchScene.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScripts.hpp"
#include "BenchPopulation.hpp"
#include "Collision.hpp"
#include "CollisionSystem.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "MovementSystem.hpp"
#include "Pilot.hpp"
#include "PilotSystem.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "SheetTextReading.hpp"
#include "SystemTime.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"
#include "TimedBehaviour.hpp"

namespace rtype::tools {

namespace {

constexpr float HALF_A_LANE = 0.5F;
constexpr float SHIPS_ACROSS = 200.0F;

/** @returns The height of the middle of the lane of the body numbered
 * @p sequence, among @p Lanes lanes of equal height. */
template <std::size_t Lanes>
float laneHeight(std::size_t sequence) {
  const auto lane = static_cast<float>(sequence % Lanes);
  return game::PLAYFIELD_HEIGHT * (lane + HALF_A_LANE) /
         static_cast<float>(Lanes);
}

/** @returns How far across the playfield the body @p index of @p count starts,
 * so the bodies leave a few at a time instead of all at once. */
float spreadAcross(std::size_t index, std::size_t count) {
  return game::PLAYFIELD_WIDTH * static_cast<float>(index) /
         static_cast<float>(count);
}

/** @returns The name of the sheet of the scripted bodies of @p population. */
std::string_view scriptedTypeOf(const BenchPopulation& population) {
  return population.waiting ? BENCH_WAITING_TYPE : BENCH_SCRIPTED_TYPE;
}

/** @returns The sheets of @p contentFolders, once @p scripts ran the scripts
 * of those folders, so a sheet can name the behaviour a script registers. */
game::EntitySheets sheetsAfterRunning(
    game::BehaviourScripts& scripts,
    std::span<const std::filesystem::path> contentFolders) {
  scripts.run(game::scriptSourcesIn(contentFolders));
  return game::EntitySheets::fromTexts(
      game::sheetTextsInFolders(contentFolders), scripts.behaviours());
}

/** @returns The total of the system named @p name among @p times, zero when
 * none bears that name. */
engine::Duration timeOf(std::span<const engine::SystemTime> times,
                        std::string_view name) {
  const auto found = std::ranges::find(times, name, &engine::SystemTime::name);
  return found == times.end() ? engine::Duration::zero() : found->total;
}

}  // namespace

BenchScene::BenchScene(BenchPopulation population, float cellSide,
                       std::span<const std::filesystem::path> contentFolders)
    : population_(population),
      scripts_(
          [this](const std::string& message) { scriptFailure_ = message; }),
      sheets_(sheetsAfterRunning(scripts_, contentFolders)),
      timedScript_(TimedBehaviourParts{
          .behaviour =
              // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
          sheets_.named(scriptedTypeOf(population)).pilot.value().behaviour,
          .spent = scriptTime_}),
      collisionCounting_(events_.subscribe<engine::Collision>(
          [this](const engine::Collision&) { ++collisionsFound_; })) {
  systems_.add(std::make_unique<game::PilotSystem>(events_));
  systems_.add(std::make_unique<game::MovementSystem>());
  systems_.add(std::make_unique<game::PlayfieldExitSystem>(events_));
  systems_.add(std::make_unique<game::CollisionSystem>(events_, cellSide),
               BENCH_COLLISION_SYSTEM_NAME);
  systems_.measureWith(&clock_);
  placeShips();
  fill();
}

void BenchScene::placeShips() {
  const game::EntitySheet& ship = sheets_.named(BENCH_SHIP_TYPE);
  for (std::size_t index = 0; index < BENCH_SHIPS; ++index) {
    world_.spawn(ship, game::Position{.x = SHIPS_ACROSS,
                                      .y = laneHeight<BENCH_LANES>(
                                          index * BENCH_LANES / BENCH_SHIPS)});
  }
}

void BenchScene::fill() {
  for (std::size_t index = 0; index < population_.scriptedBodies; ++index) {
    spawnScripted(spreadAcross(index, population_.scriptedBodies));
  }
  for (std::size_t index = 0; index < population_.bullets; ++index) {
    spawnBullet(spreadAcross(index, population_.bullets));
  }
  timeScriptedBodies();
}

void BenchScene::spawnScripted(float across) {
  world_.spawn(
      sheets_.named(scriptedTypeOf(population_)),
      game::Position{.x = across,
                     .y = laneHeight<BENCH_LANES>(scriptedSpawned_++)});
}

void BenchScene::spawnBullet(float across) {
  world_.spawn(
      sheets_.named(BENCH_BULLET_TYPE),
      game::Position{.x = across,
                     .y = laneHeight<BENCH_BULLET_LANES>(bulletsSpawned_++)});
}

void BenchScene::timeScriptedBodies() {
  world_.components().forEach<game::Pilot, game::EntityType>(
      [this](engine::Entity, game::Pilot& pilot, const game::EntityType& type) {
        if (type == game::EntityType::Enemy) {
          pilot.behaviour = timedScript_;
        }
      });
}

void BenchScene::replaceWhatLeft() {
  std::size_t scripted = 0;
  std::size_t bullets = 0;
  world_.components().forEach<game::Position, game::EntityType>(
      [&scripted, &bullets](engine::Entity, const game::Position&,
                            const game::EntityType& type) {
        scripted += type == game::EntityType::Enemy ? 1U : 0U;
        bullets += type == game::EntityType::EnemyMissile ? 1U : 0U;
      });
  const bool bodiesLeft = scripted < population_.scriptedBodies;
  for (; scripted < population_.scriptedBodies; ++scripted) {
    spawnScripted(game::PLAYFIELD_WIDTH);
  }
  for (; bullets < population_.bullets; ++bullets) {
    spawnBullet(game::PLAYFIELD_WIDTH);
  }
  if (bodiesLeft) {
    timeScriptedBodies();
  }
}

TickSample BenchScene::runTick() {
  scriptTime_ = engine::Duration::zero();
  collisionsFound_ = 0;
  scripts_.machine().beginTick();
  const auto start = std::chrono::steady_clock::now();
  systems_.run(world_.components(), engine::SIMULATION_TICK_DURATION);
  events_.dispatch();
  replaceWhatLeft();
  const auto whole = std::chrono::duration_cast<engine::Duration>(
      std::chrono::steady_clock::now() - start);
  return TickSample{.whole = whole,
                    .scripts = scriptTime_,
                    .collisions = timeOf(systems_.takeSystemTimes(),
                                         BENCH_COLLISION_SYSTEM_NAME),
                    .collisionsFound = collisionsFound_};
}

std::size_t BenchScene::scriptMemory() {
  return scripts_.machine().memoryUsed();
}

std::size_t BenchScene::bodiesInPlay() const { return world_.entityCount(); }

const std::string& BenchScene::scriptFailure() const { return scriptFailure_; }

}  // namespace rtype::tools
