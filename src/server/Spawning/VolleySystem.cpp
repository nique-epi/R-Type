#include "VolleySystem.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include "BodyPlacement.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "Intent.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "VolleyPlacement.hpp"
#include "VolleySheet.hpp"
#include "VolleyShot.hpp"
#include "VolleySystemConstants.hpp"
#include "VolleySystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

VolleySystem::VolleySystem(VolleySystemParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      types_(&parts.types.get()),
      logger_(std::string{VOLLEY_SYSTEM_LOGGER_NAME}) {}

void VolleySystem::onTick() {
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
        const game::EntitySheet* const sheet = types_->numbered(type.number);
        const auto* const position = components.get<game::Position>(body);
        if (!intent.startsAction.has_value() || sheet == nullptr ||
            position == nullptr) {
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
  if (sheet == nullptr) {
    return;
  }
  for (std::size_t index = 0; index < volley.count; ++index) {
    const std::optional<game::BodyPlacement> placement =
        game::volleyPlacement(volley, shot.center, index);
    if (!placement.has_value()) {
      continue;
    }
    const std::optional<network::EntitySpawned> body =
        spawnAt(*sheet, *placement);
    if (!body.has_value()) {
      return;
    }
    announce(*body);
  }
}

std::optional<network::EntitySpawned> VolleySystem::spawnAt(
    const game::EntitySheet& sheet, game::BodyPlacement placement) {
  try {
    return entitySpawnedOf(world_->spawnFrom(sheet, placement),
                           game::EntityTypeNumber{.number = sheet.number},
                           placement.position);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("the rest of a volley is skipped: every network id was given");
    return std::nullopt;
  }
}

void VolleySystem::announce(const network::EntitySpawned& body) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, body);
  }
}

}  // namespace rtype::server
