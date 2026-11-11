#include "EntityMessageQueue.hpp"
#include <algorithm>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>
#include "BossStatus.hpp"
#include "DeltaWorldState.hpp"
#include "EffectPlayed.hpp"
#include "EnemyAction.hpp"
#include "EntityDestroyed.hpp"
#include "EntityMessage.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "GameLoadedReminder.hpp"
#include "ISessionOutbox.hpp"
#include "LevelStatus.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PlayerLeft.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @brief Hands every message of these types that arrives to @p keep. */
template <typename... Messages, typename Keep>
void keepEachOf(network::MessageDispatcher& dispatcher, Keep keep) {
  (dispatcher.registerHandler<Messages>(
       [keep](const network::MessageOrigin&, const Messages& message) {
         keep(message);
       }),
   ...);
}

}  // namespace

EntityMessageQueue::EntityMessageQueue(network::MessageDispatcher& dispatcher,
                                       WaitingRoom& room,
                                       GameLoadedReminder& reminder)
    : room_(&room), reminder_(&reminder) {
  registerStateHandlers(dispatcher);
  keepEachOf<network::EntitySpawned, network::EntityDestroyed,
             network::PlayerLeft, network::GameEvent, network::EnemyAction,
             network::BossStatus, network::EffectPlayed, network::LevelStatus>(
      dispatcher, [this](EntityMessage message) { keep(std::move(message)); });
  dispatcher.registerHandler<network::GameEnded>(
      [this](const network::MessageOrigin&, const network::GameEnded& ended) {
        if (room_->gameStarted()) {
          keep(ended);
          room_->endGame();
        }
      });
}

void EntityMessageQueue::registerStateHandlers(
    network::MessageDispatcher& dispatcher) {
  dispatcher.registerHandler<network::WorldState>(
      [this](const network::MessageOrigin&, const network::WorldState& state) {
        reminder_->worldStateArrived();
        room_->startGame();
        keep(state);
      });
  dispatcher.registerHandler<network::DeltaWorldState>(
      [this](const network::MessageOrigin&,
             const network::DeltaWorldState& part) {
        reminder_->worldStateArrived();
        room_->startGame();
        if (std::optional<network::WorldState> state = assembler_.add(part)) {
          keep(std::move(*state));
        }
      });
}

void EntityMessageQueue::drainEntityMessages(
    std::vector<EntityMessage>& destination) {
  std::ranges::move(messages_, std::back_inserter(destination));
  messages_.clear();
}

void EntityMessageQueue::sessionOpened(
    [[maybe_unused]] const network::LobbyPlayer& ownPlayer,
    [[maybe_unused]] ISessionOutbox& outbox) {}

void EntityMessageQueue::sessionClosed() {
  messages_.clear();
  assembler_.clear();
}

void EntityMessageQueue::sessionHeld(
    [[maybe_unused]] engine::Duration elapsed) {}

void EntityMessageQueue::keep(EntityMessage message) {
  if (room_->gameStarted()) {
    messages_.push_back(std::move(message));
  }
}

}  // namespace rtype::client
