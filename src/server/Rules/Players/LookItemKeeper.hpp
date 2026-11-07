#pragma once

#include <cstdint>
#include <functional>
#include <span>
#include <vector>
#include "Entity.hpp"
#include "LookItemKeeperParts.hpp"
#include "OutgoingDatagram.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::engine {

class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

class World;
struct LookItemTouched;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @brief A pilot who touched a look item during a tick. */
struct LookItemClaim {
  engine::Entity item{};
  std::uint32_t itemId{0};
  std::uint8_t lookNumber{0};
  std::uint8_t playerNumber{0};
  std::uint32_t shipId{0};
};

/**
 * @brief Gives each look item a ship touched to one pilot.
 *
 * Every LookItemTouched of a tick is recorded; onTick(), called at the start
 * of the next tick before the world moves, gives each item to the lowest
 * player number among the pilots who touched it and still fly the ship that
 * touched it, removes the item (EntityDestruction, Removed) and tells that
 * pilot alone, with a guaranteed LookItemWon. An item no such pilot is left
 * for stays in the world. Subscribes on construction, unsubscribes on
 * destruction. Not thread-safe: use it from the simulation thread only.
 */
class LookItemKeeper {
 public:
  explicit LookItemKeeper(LookItemKeeperParts parts);
  ~LookItemKeeper();

  LookItemKeeper(const LookItemKeeper&) = delete;
  LookItemKeeper& operator=(const LookItemKeeper&) = delete;
  LookItemKeeper(LookItemKeeper&&) = delete;
  LookItemKeeper& operator=(LookItemKeeper&&) = delete;

  /** @brief Gives every item touched since the previous call. */
  void onTick();

 private:
  void onTouched(const game::LookItemTouched& touched);
  void giveItem(std::span<const LookItemClaim> claims);

  engine::EventBus& events_;
  game::World& world_;
  PlayerRoster& roster_;
  std::function<void(network::OutgoingDatagram)> send_;
  std::vector<LookItemClaim> claims_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::server
