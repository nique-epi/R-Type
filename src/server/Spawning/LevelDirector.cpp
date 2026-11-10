#include "LevelDirector.hpp"
#include <string>
#include <utility>
#include <vector>
#include "BehaviourScripts.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyCount.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "LevelChange.hpp"
#include "LevelDirectorConstants.hpp"
#include "LevelDirectorParts.hpp"
#include "LevelProgress.hpp"
#include "LuauLevelRunParts.hpp"
#include "LuauLevelRunState.hpp"
#include "LuauLevels.hpp"
#include "NetworkIdentity.hpp"
#include "ServerException.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::vector<int> stagesOfLevel(const game::LuauLevels& levels,
                               const std::string& level) {
  const std::vector<int>* const stages = levels.stagesOf(level);
  if (stages == nullptr) {
    throw UnknownLevelException(level, levels.names());
  }
  return *stages;
}

LevelDirector::LevelDirector(LevelDirectorParts parts)
    : parts_(std::move(parts)),
      run_(game::LuauLevelRunParts{
          .machine = parts_.scripts.get().machine(),
          .levels = parts_.scripts.get().levels(),
          .stages = stagesOfLevel(parts_.scripts.get().levels(), parts_.level),
          .progress = progress_,
          .components = parts_.world.get().components()}),
      logger_(std::string{LEVEL_DIRECTOR_LOGGER_NAME}) {}

void LevelDirector::onTick() {
  const bool hadFailed = failed();
  run_.step();
  if (failed() && !hadFailed) {
    logger_.warn("the level ", parts_.level, " failed: ", run_.failure());
  }
  progress_.scroll.advance(engine::SIMULATION_TICK_DURATION);
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
  announcedBiome_.reset();
  logger_.info("the level ", parts_.level,
               " restarts from its last checkpoint, at ",
               progress_.scroll.position(), " units of scroll");
  return true;
}

const game::LevelProgress& LevelDirector::progress() const { return progress_; }

void LevelDirector::announceChange() {
  const bool moving = progress_.scroll.speed() > 0.0F;
  if (announcedBiome_.has_value() && *announcedBiome_ == progress_.biome &&
      announcedMoving_ == moving) {
    return;
  }
  announcedBiome_ = progress_.biome;
  announcedMoving_ = moving;
  parts_.events.get().publish(
      game::LevelChange{.biome = progress_.biome,
                        .scrollPosition = progress_.scroll.position(),
                        .scrollSpeed = progress_.scroll.speed()});
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
