#include "ScriptedBodies.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "BehaviourScripts.hpp"
#include "BodyPlacement.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EntityTypeNumber.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "ScriptMachine.hpp"
#include "ScriptedBodiesConstants.hpp"
#include "ScriptedBodiesParts.hpp"
#include "ScriptedBodyOrder.hpp"
#include "ScriptedSpawn.hpp"

namespace rtype::server {

namespace {

/** @returns The sheet of @p sheets named @p name; nullptr when none is. */
const game::EntitySheet* sheetNamed(std::span<const game::EntitySheet> sheets,
                                    std::string_view name) {
  for (const game::EntitySheet& sheet : sheets) {
    if (sheet.name == name) {
      return &sheet;
    }
  }
  return nullptr;
}

}  // namespace

ScriptedBodies::ScriptedBodies(ScriptedBodiesParts parts)
    : parts_(std::move(parts)),
      logger_(std::string{SCRIPTED_BODIES_LOGGER_NAME}) {
  if (parts_.scripts != nullptr) {
    parts_.scripts->attachWorld(*this);
  }
}

ScriptedBodies::~ScriptedBodies() {
  if (parts_.scripts != nullptr) {
    parts_.scripts->detachWorld(*this);
  }
}

void ScriptedBodies::beginTick() {
  if (parts_.scripts != nullptr) {
    parts_.scripts->machine().beginTick();
  }
}

std::uint32_t ScriptedBodies::spawn(const game::ScriptedSpawn& spawn) {
  if (orders_.size() >= MAXIMUM_SCRIPTED_SPAWNS_PER_TICK) {
    throw engine::ScriptException("too many bodies created in one tick");
  }
  const game::EntitySheet* const sheet =
      sheetNamed(parts_.types.get().all(), spawn.type);
  if (sheet == nullptr) {
    throw engine::ScriptException("no sheet is named " +
                                  std::string{spawn.type});
  }
  if (!sheet->pilot.has_value()) {
    throw engine::ScriptException("a script cannot create a ship");
  }
  const std::uint32_t networkId = parts_.world.get().reserveNetworkId();
  orders_.push_back(ScriptedBodyOrder{
      .sheet = *sheet, .position = spawn.position, .networkId = networkId});
  return networkId;
}

void ScriptedBodies::destroy(std::uint32_t networkId) {
  if (removals_.size() >= MAXIMUM_SCRIPTED_REMOVALS_PER_TICK) {
    throw engine::ScriptException("too many bodies removed in one tick");
  }
  removals_.push_back(networkId);
}

void ScriptedBodies::warnWhenOverBudget() {
  if (!budgetWarned_ && parts_.scripts != nullptr &&
      !parts_.scripts->machine().hasTickBudget()) {
    budgetWarned_ = true;
    logger_.warn("the scripts used the whole of their budget for a tick");
  }
}

void ScriptedBodies::onTick() {
  warnWhenOverBudget();
  for (const ScriptedBodyOrder& order : orders_) {
    appear(order);
  }
  orders_.clear();
  removeAsked();
}

void ScriptedBodies::appear(const ScriptedBodyOrder& order) {
  const game::EntitySheet& sheet = order.sheet.get();
  parts_.world.get().spawnFrom(
      sheet, game::BodyPlacement{.position = order.position,
                                 .origin = order.position,
                                 .reservedNetworkId = order.networkId});
  const network::EntitySpawned spawned = entitySpawnedOf(
      order.networkId, game::EntityTypeNumber{.number = sheet.number},
      order.position);
  for (ConnectedPlayer* const player : parts_.roster.get().connected()) {
    sendEntitySpawned(parts_.send, *player, spawned);
  }
}

void ScriptedBodies::removeAsked() {
  if (removals_.empty()) {
    return;
  }
  std::ranges::sort(removals_);
  engine::ComponentRegistry& components = parts_.world.get().components();
  components.forEach<game::NetworkIdentity, game::EntityType>(
      [this, &components](engine::Entity body,
                          const game::NetworkIdentity& identity,
                          const game::EntityType& type) {
        if (type == game::EntityType::Player ||
            !std::ranges::binary_search(removals_, identity.networkId)) {
          return;
        }
        parts_.events.get().publish(
            game::EntityDestruction{.networkId = identity.networkId,
                                    .cause = game::DestructionCause::Removed});
        components.destroy(body);
      });
  removals_.clear();
}

}  // namespace rtype::server
