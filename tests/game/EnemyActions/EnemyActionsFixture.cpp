#include "EnemyActionsFixture.hpp"
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EnemyActionStarted.hpp"
#include "EnemyKind.hpp"
#include "Entity.hpp"
#include "EventBus.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "WorldTestConstants.hpp"

namespace rtype::game::testing {

EnemyActionsFixture::EnemyActionsFixture()
    : subscription_(events_.subscribe<EnemyActionStarted>(
          [this](const EnemyActionStarted& event) {
            delivered_.push_back(event);
          })) {
  enemyNetworkId_ =
      world_.spawnBydo(Position{.x = SOME_X, .y = SOME_Y}, EnemyKind::Reaper);
  world_.components().forEach<NetworkIdentity, NetworkIdentity>(
      [this](engine::Entity entity, const NetworkIdentity& identity,
             const NetworkIdentity&) {
        if (identity.networkId == enemyNetworkId_) {
          enemy_ = entity;
        }
      });
}

EnemyActionsFixture::~EnemyActionsFixture() {
  events_.unsubscribe(subscription_);
}

engine::Entity EnemyActionsFixture::enemy() const { return enemy_; }

std::uint32_t EnemyActionsFixture::enemyNetworkId() const {
  return enemyNetworkId_;
}

engine::ComponentRegistry& EnemyActionsFixture::components() {
  return world_.components();
}

engine::EventBus& EnemyActionsFixture::events() { return events_; }

const std::vector<EnemyActionStarted>& EnemyActionsFixture::delivered() {
  events_.dispatch();
  return delivered_;
}

}  // namespace rtype::game::testing
