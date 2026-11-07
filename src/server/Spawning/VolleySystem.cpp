#include "VolleySystem.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include "BodyPlacement.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EntityTypeNumber.hpp"
#include "Gun.hpp"
#include "Intent.hpp"
#include "NetworkIdentity.hpp"
#include "PercentConstants.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "Summoning.hpp"
#include "VolleyPlacement.hpp"
#include "VolleySheet.hpp"
#include "VolleyShot.hpp"
#include "VolleySystemConstants.hpp"
#include "VolleySystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns @p center moved at random by up to the jitter of @p volley on
 * each axis, drawn from @p draws. */
game::Position jitteredCenter(const game::VolleySheet& volley,
                              game::Position center, std::mt19937& draws) {
  auto moveOf = [&draws](float jitter) {
    return jitter > 0.0F
               ? std::uniform_real_distribution<float>{-jitter, jitter}(draws)
               : 0.0F;
  };
  return game::Position{.x = center.x + moveOf(volley.jitterX),
                        .y = center.y + moveOf(volley.jitterY)};
}

/** @returns @p placement moved up or down, origin included, so its body
 * stands within SUMMONED_ROW_MARGIN of the play area. */
game::BodyPlacement keptInPlay(game::BodyPlacement placement) {
  const float shift = std::clamp(placement.position.y, SUMMONED_ROW_MARGIN,
                                 game::PLAY_AREA_HEIGHT - SUMMONED_ROW_MARGIN) -
                      placement.position.y;
  placement.position.y += shift;
  placement.origin.y += shift;
  return placement;
}

/** @brief Gives the body of network id @p networkId in @p components its
 * Summoning, and puts its first shot off by as long: its Gun counts from the
 * tick it takes part. */
void markSummoned(engine::ComponentRegistry& components,
                  std::uint32_t networkId) {
  std::optional<engine::Entity> summoned;
  components.forEach<game::NetworkIdentity, game::NetworkIdentity>(
      [&summoned, networkId](engine::Entity body,
                             const game::NetworkIdentity& identity,
                             const game::NetworkIdentity&) {
        if (identity.networkId == networkId) {
          summoned = body;
        }
      });
  if (!summoned.has_value()) {
    return;
  }
  components.add(*summoned, game::Summoning{.ticksLeft = SUMMONING_TICKS});
  if (auto* const gun = components.get<game::Gun>(*summoned)) {
    gun->ticksUntilReady += SUMMONING_TICKS;
  }
}

}  // namespace

VolleySystem::VolleySystem(VolleySystemParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      types_(&parts.types.get()),
      logger_(std::string{VOLLEY_SYSTEM_LOGGER_NAME}),
      draws_(parts.seed) {}

void VolleySystem::onTick() {
  wakeSummoned();
  collectShots();
  for (const VolleyShot& shot : shots_) {
    fire(shot);
  }
}

void VolleySystem::collectShots() {
  shots_.clear();
  engine::ComponentRegistry& components = world_->components();
  components.forEach<game::Intent, game::EntityTypeNumber>(
      [this, &components](engine::Entity body, const game::Intent& intent,
                          const game::EntityTypeNumber& type) {
        if (!intent.startsAction.has_value()) {
          return;
        }
        const game::EntitySheet* const sheet = types_->numbered(type.number);
        const auto* const position = components.get<game::Position>(body);
        if (sheet == nullptr || position == nullptr) {
          return;
        }
        for (const game::VolleySheet& volley : sheet->volleys) {
          if (volley.on == *intent.startsAction) {
            shots_.push_back(VolleyShot{.center = *position, .volley = volley});
          }
        }
      });
}

void VolleySystem::fire(const VolleyShot& shot) {
  const game::VolleySheet& volley = shot.volley.get();
  const game::EntitySheet* const sheet = types_->numbered(volley.bodyNumber);
  if (sheet == nullptr ||
      (volley.chancePercent < game::VOLLEY_SURE_CHANCE &&
       std::uniform_int_distribution<int>{1, game::WHOLE_PERCENT}(draws_) >
           volley.chancePercent)) {
    return;
  }
  const game::Position center = jitteredCenter(volley, shot.center, draws_);
  for (std::size_t index = 0; index < volley.count; ++index) {
    const std::optional<game::BodyPlacement> placement =
        game::volleyPlacement(volley, center, index);
    if (!placement.has_value()) {
      continue;
    }
    const std::optional<network::EntitySpawned> body = spawnAt(
        *sheet, sheet->side == game::EntityType::Enemy ? keptInPlay(*placement)
                                                       : *placement);
    if (!body.has_value()) {
      return;
    }
    announce(*body);
  }
}

std::optional<network::EntitySpawned> VolleySystem::spawnAt(
    const game::EntitySheet& sheet, game::BodyPlacement placement) {
  try {
    const std::uint32_t networkId = world_->spawnFrom(sheet, placement);
    if (sheet.side == game::EntityType::Enemy) {
      markSummoned(world_->components(), networkId);
    }
    return entitySpawnedOf(networkId,
                           game::EntityTypeNumber{.number = sheet.number},
                           placement.position);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("the rest of a volley is skipped: every network id was given");
    return std::nullopt;
  }
}

void VolleySystem::wakeSummoned() {
  awoken_.clear();
  engine::ComponentRegistry& components = world_->components();
  components.forEach<game::Summoning, game::Summoning>(
      [this](engine::Entity body, game::Summoning& summoning,
             game::Summoning&) {
        if (--summoning.ticksLeft == 0) {
          awoken_.push_back(body);
        }
      });
  for (const engine::Entity body : awoken_) {
    components.remove<game::Summoning>(body);
  }
}

void VolleySystem::announce(const network::EntitySpawned& body) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, body);
  }
}

}  // namespace rtype::server
