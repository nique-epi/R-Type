#include "DropSystem.hpp"
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "CrystalDropped.hpp"
#include "DropSystemParts.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

DropSystem::DropSystem(DropSystemParts parts)
    : parts_(std::move(parts)),
      subscription_(parts_.events.get().subscribe<game::CrystalDropped>(
          [this](const game::CrystalDropped& dropped) {
            due_.push_back(dropped);
          })) {}

DropSystem::~DropSystem() { parts_.events.get().unsubscribe(subscription_); }

void DropSystem::onTick() {
  const std::vector<game::CrystalDropped> due = std::exchange(due_, {});
  for (const game::CrystalDropped& dropped : due) {
    drop(dropped);
  }
}

void DropSystem::drop(const game::CrystalDropped& dropped) const {
  const std::optional<network::EntitySpawned> spawned = spawnCrystal(dropped);
  if (!spawned.has_value()) {
    return;
  }
  PlayerRoster& roster = parts_.roster.get();
  for (ConnectedPlayer* const player : roster.connected()) {
    sendEntitySpawned(parts_.send, *player, *spawned);
  }
}

std::optional<network::EntitySpawned> DropSystem::spawnCrystal(
    const game::CrystalDropped& dropped) const {
  const game::EntitySheets& types = parts_.types.get();
  const game::EntitySheet* const sheet = types.numbered(dropped.sheetNumber);
  if (sheet == nullptr) {
    return std::nullopt;
  }
  try {
    const std::uint32_t networkId =
        parts_.world.get().spawn(*sheet, dropped.position);
    return entitySpawnedOf(networkId,
                           game::EntityTypeNumber{.number = sheet->number},
                           dropped.position);
  } catch (const engine::NetworkIdExhaustedException&) {
    return std::nullopt;
  }
}

}  // namespace rtype::server
