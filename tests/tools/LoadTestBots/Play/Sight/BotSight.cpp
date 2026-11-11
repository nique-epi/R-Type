#include "BotSight.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <variant>
#include "EntityKind.hpp"
#include "EntityMessage.hpp"
#include "EntityType.hpp"
#include "GameEnded.hpp"
#include "IEntityTypeTable.hpp"
#include "NumberedEntityType.hpp"
#include "OwnShip.hpp"
#include "SeenEntity.hpp"
#include "SightFigures.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::tools {

namespace {

/** @returns The kind of @p type; nothing for a type a bot does not look at:
 * unknown, a ship or a missile of a player. */
std::optional<EntityKind> kindOf(const game::NumberedEntityType* type) {
  if (type == nullptr) {
    return std::nullopt;
  }
  switch (type->side) {
    case game::EntityType::Enemy:
      return EntityKind::Enemy;
    case game::EntityType::EnemyMissile:
      return EntityKind::EnemyMissile;
    case game::EntityType::LookItem:
      return EntityKind::Item;
    case game::EntityType::Player:
    case game::EntityType::PlayerMissile:
      break;
  }
  return std::nullopt;
}

/** @returns @p seen with the velocity that brought it from @p before over
 * @p seconds. */
SeenEntity movedFrom(SeenEntity seen, const SeenEntity& before, float seconds) {
  seen.velocityX = (seen.x - before.x) / seconds;
  seen.velocityY = (seen.y - before.y) / seconds;
  return seen;
}

}  // namespace

BotSight::BotSight(const client::IEntityTypeTable& types) : types_(&types) {}

void BotSight::read(const client::EntityMessage& message,
                    std::uint8_t playerNumber) {
  if (const auto* state = std::get_if<network::WorldState>(&message)) {
    startTick(state->tick);
    readPlayer(*state, playerNumber);
    addEntities(*state);
  } else if (std::holds_alternative<network::GameEnded>(message)) {
    endGame();
  }
}

const SightFigures& BotSight::figures() const { return figures_; }

std::span<const SeenEntity> BotSight::entities() const { return entities_; }

void BotSight::startTick(std::uint32_t tick) {
  if (figures_.tick == tick) {
    return;
  }
  previous_.swap(entities_);
  entities_.clear();
  previousTick_ = figures_.tick.value_or(0);
  figures_.tick = tick;
}

void BotSight::readPlayer(const network::WorldState& state,
                          std::uint8_t playerNumber) {
  const auto player = std::ranges::find(state.players, playerNumber,
                                        &network::PlayerState::playerNumber);
  if (player == state.players.end()) {
    return;
  }
  if (player->lives < figures_.lives) {
    figures_.livesLost += static_cast<std::uint32_t>(figures_.lives) -
                          static_cast<std::uint32_t>(player->lives);
  }
  figures_.lives = player->lives;
  figures_.invulnerable = player->invulnerable;
  figures_.inPlay = player->lives > 0;
  figures_.scoreEarned = scoreOfEndedGames_ + player->score;
  if (const auto own = client::ownShipOf(state, playerNumber)) {
    figures_.shipX = own->position.x;
    figures_.shipY = own->position.y;
  }
}

void BotSight::addEntities(const network::WorldState& state) {
  const float seconds = static_cast<float>(state.tick - previousTick_) /
                        static_cast<float>(engine::SIMULATION_TICKS_PER_SECOND);
  for (const network::EntityState& entity : state.entities) {
    const game::NumberedEntityType* type =
        types_->entryNumbered(entity.entityType);
    const std::optional<EntityKind> kind = kindOf(type);
    if (!kind.has_value()) {
      continue;
    }
    const SeenEntity seen{
        .entityId = entity.entityId,
        .kind = *kind,
        .x = entity.x,
        .y = entity.y,
        .width = type->hitbox.width,
        .height = type->hitbox.height,
    };
    const auto before =
        std::ranges::find(previous_, entity.entityId, &SeenEntity::entityId);
    entities_.push_back(
        before != previous_.end() ? movedFrom(seen, *before, seconds) : seen);
  }
}

void BotSight::endGame() {
  scoreOfEndedGames_ = figures_.scoreEarned;
  figures_.tick.reset();
  figures_.inPlay = false;
  figures_.lives = 0;
  entities_.clear();
  previous_.clear();
}

}  // namespace rtype::tools
