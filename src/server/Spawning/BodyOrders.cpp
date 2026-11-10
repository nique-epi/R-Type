#include "BodyOrders.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "BehaviourScripts.hpp"
#include "BodyOrder.hpp"
#include "BodyOrdersConstants.hpp"
#include "BodyOrdersParts.hpp"
#include "BodyPlacement.hpp"
#include "ComponentRegistry.hpp"
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
#include "Position.hpp"
#include "ScriptMachine.hpp"

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

BodyOrders::BodyOrders(BodyOrdersParts parts)
    : parts_(std::move(parts)), logger_(std::string{BODY_ORDERS_LOGGER_NAME}) {
  if (parts_.scripts != nullptr) {
    parts_.scripts->attachOrders(*this);
  }
}

BodyOrders::~BodyOrders() {
  if (parts_.scripts != nullptr) {
    parts_.scripts->detachOrders(*this);
  }
}

std::uint32_t BodyOrders::spawn(std::string_view type,
                                game::Position position) {
  if (orders_.size() >= MAXIMUM_SPAWN_ORDERS_PER_TICK) {
    throw engine::ScriptException("too many bodies created in one tick");
  }
  const game::EntitySheet* const sheet =
      sheetNamed(parts_.types.get().all(), type);
  if (sheet == nullptr) {
    throw engine::ScriptException("no sheet is named " + std::string{type});
  }
  if (!sheet->pilot.has_value()) {
    throw engine::ScriptException("a script cannot create a ship");
  }
  const std::uint32_t networkId = parts_.world.get().reserveNetworkId();
  orders_.push_back(
      BodyOrder{.sheet = *sheet, .position = position, .networkId = networkId});
  return networkId;
}

void BodyOrders::destroy(std::uint32_t networkId) {
  if (removals_.size() >= MAXIMUM_REMOVAL_ORDERS_PER_TICK) {
    throw engine::ScriptException("too many bodies removed in one tick");
  }
  removals_.push_back(networkId);
}

void BodyOrders::warnWhenOverBudget() {
  if (!budgetWarned_ && parts_.scripts != nullptr &&
      !parts_.scripts->machine().hasTickBudget()) {
    budgetWarned_ = true;
    logger_.warn("the scripts used the whole of their budget for a tick");
  }
}

void BodyOrders::onTick() {
  warnWhenOverBudget();
  for (const BodyOrder& order : orders_) {
    appear(order);
  }
  orders_.clear();
  removeAsked();
  if (parts_.scripts != nullptr) {
    parts_.scripts->machine().beginTick();
  }
}

void BodyOrders::appear(const BodyOrder& order) const {
  const game::EntitySheet& sheet = order.sheet.get();
  parts_.world.get().spawnFrom(
      sheet, game::BodyPlacement{.position = order.position,
                                 .origin = order.position,
                                 .reservedNetworkId = order.networkId});
  const network::EntitySpawned spawned = entitySpawnedOf(
      order.networkId, game::EntityTypeNumber{.number = sheet.number},
      order.position);
  PlayerRoster& roster = parts_.roster.get();
  for (ConnectedPlayer* const player : roster.connected()) {
    sendEntitySpawned(parts_.send, *player, spawned);
  }
}

void BodyOrders::removeAsked() {
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
