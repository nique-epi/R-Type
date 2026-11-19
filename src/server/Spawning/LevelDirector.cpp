#include "LevelDirector.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "BehaviourScripts.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyCount.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "LevelChange.hpp"
#include "LevelCheckpoint.hpp"
#include "LevelDirectorConstants.hpp"
#include "LevelDirectorParts.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevelRunParts.hpp"
#include "LuauLevelRunState.hpp"
#include "LuauLevels.hpp"
#include "NetworkIdentity.hpp"
#include "PlayBand.hpp"
#include "ScrollingBodies.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The milliseconds left of the transition @p progress plays, 0 when
 * it plays none; LONGEST_LEVEL_TRANSITION keeps them within 16 bits. */
/** @brief Puts @p progress inside the biome @p entry opens in, begun
 * OPENING_BIOME_LEAD before the start of the scroll, so its entrance has left
 * the screen; nothing for a level that opens in no biome. */
void openInBiome(game::LevelProgress& progress,
                 const game::LuauLevelEntry& entry) {
  if (entry.openingBiome.has_value()) {
    progress.biome = *entry.openingBiome;
    progress.biomeSince = progress.scroll.position() - OPENING_BIOME_LEAD;
  }
}

std::uint16_t transitionLeftOf(const game::LevelProgress& progress) {
  if (!progress.transition.has_value()) {
    return 0;
  }
  const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(
      engine::SIMULATION_TICK_DURATION * progress.transition->ticksLeft);
  return static_cast<std::uint16_t>(left.count());
}

}  // namespace

const game::LuauLevelEntry& entryOfLevel(const game::LuauLevels& levels,
                                         const std::string& level) {
  const game::LuauLevelEntry* const entry = levels.entryOf(level);
  if (entry == nullptr) {
    throw UnknownLevelException(level, levels.names());
  }
  return *entry;
}

std::optional<game::LevelCheckpoint> startOfStage(
    const game::LuauLevelEntry& entry, const std::string& level,
    std::size_t stage) {
  if (stage == FIRST_LEVEL_STAGE) {
    return std::nullopt;
  }
  const auto declared = entry.starts.find(stage - FIRST_LEVEL_STAGE);
  if (declared == entry.starts.end()) {
    throw UnknownLevelStartException(level, stage);
  }
  return declared->second;
}

LevelDirector::LevelDirector(LevelDirectorParts parts)
    : parts_(std::move(parts)),
      entry_(entryOfLevel(parts_.scripts.get().levels(), parts_.level)),
      run_(game::LuauLevelRunParts{
          .machine = parts_.scripts.get().machine(),
          .levels = parts_.scripts.get().levels(),
          .entry = entry_,
          .progress = progress_,
          .components = parts_.world.get().components()}),
      placer_(entry_.get().placements),
      logger_(std::string{LEVEL_DIRECTOR_LOGGER_NAME}) {
  progress_.checkpoint =
      startOfStage(entry_, parts_.level, parts_.startingStage);
  if (progress_.checkpoint.has_value()) {
    restartFromCheckpoint();
  } else {
    openInBiome(progress_, entry_);
  }
}

void LevelDirector::onTick() {
  const bool hadFailed = failed();
  run_.step();
  if (failed() && !hadFailed) {
    logger_.warn("the level ", parts_.level, " failed: ", run_.failure());
  }
  spawnDuePlacements();
  const float scrolledBefore = progress_.scroll.position();
  progress_.scroll.advance(engine::SIMULATION_TICK_DURATION);
  game::carryAlongTheScroll(parts_.world.get().components(),
                            progress_.scroll.position() - scrolledBefore);
  advanceTransition();
  announceChange();
}

bool LevelDirector::cleared() const {
  return run_.state() == game::LuauLevelRunState::Cleared &&
         game::enemiesIn(parts_.world.get().components()) == 0;
}

bool LevelDirector::failed() const {
  return run_.state() == game::LuauLevelRunState::Failed;
}

bool LevelDirector::restartFromCheckpoint() {
  if (!run_.restartFromCheckpoint()) {
    return false;
  }
  removeEveryBodyButTheShips();
  placer_.restartAt(progress_.scroll.position());
  progress_.transition.reset();
  announcedBiome_.reset();
  logger_.info("the level ", parts_.level,
               " restarts from its last checkpoint, at ",
               progress_.scroll.position(), " units of scroll");
  return true;
}

const game::LevelProgress& LevelDirector::progress() const { return progress_; }

game::PlayBand LevelDirector::band() const {
  const auto found = entry_.get().bands.find(progress_.biome);
  return found == entry_.get().bands.end() ? game::PlayBand{} : found->second;
}

void LevelDirector::spawnDuePlacements() {
  if (!progress_.placedEnemiesAsked) {
    return;
  }
  placer_.spawnDue(progress_.scroll.position(), parts_.orders.get());
}

void LevelDirector::advanceTransition() {
  if (!progress_.transition.has_value()) {
    return;
  }
  game::LevelTransition& transition = *progress_.transition;
  if (transition.ticksLeft > 0) {
    --transition.ticksLeft;
  }
  if (transition.ticksLeft == 0) {
    progress_.transition.reset();
    progress_.scroll.resume();
  }
}

void LevelDirector::announceChange() {
  const bool moving = progress_.scroll.speed() > 0.0F;
  const bool transitioning = progress_.transition.has_value();
  if (announcedBiome_.has_value() && *announcedBiome_ == progress_.biome &&
      announcedMoving_ == moving && announcedTransitioning_ == transitioning) {
    return;
  }
  announcedBiome_ = progress_.biome;
  announcedMoving_ = moving;
  announcedTransitioning_ = transitioning;
  parts_.events.get().publish(
      game::LevelChange{.biome = progress_.biome,
                        .scrollPosition = progress_.scroll.position(),
                        .scrollSpeed = progress_.scroll.speed(),
                        .biomeSince = progress_.biomeSince,
                        .transitionLeft = transitionLeftOf(progress_),
                        .band = band()});
}

void LevelDirector::removeEveryBodyButTheShips() const {
  engine::ComponentRegistry& components = parts_.world.get().components();
  engine::EventBus& events = parts_.events.get();
  components.forEach<game::NetworkIdentity, game::EntityType>(
      [&components, &events](engine::Entity body,
                             const game::NetworkIdentity& identity,
                             const game::EntityType& type) {
        if (type == game::EntityType::Player) {
          return;
        }
        events.publish(
            game::EntityDestruction{.networkId = identity.networkId,
                                    .cause = game::DestructionCause::Removed});
        components.destroy(body);
      });
}

}  // namespace rtype::server
