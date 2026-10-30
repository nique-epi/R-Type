#include "EntityMessageQueue.hpp"
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntityMessage.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"
#include "WorldState.hpp"

namespace rtype::client {

EntityMessageQueue::EntityMessageQueue(network::MessageDispatcher& dispatcher,
                                       WaitingRoom& room)
    : room_(&room) {
  dispatcher.registerHandler<network::WorldState>(
      [this](const network::MessageOrigin&, const network::WorldState& state) {
        room_->startGame();
        keep(state);
      });
  dispatcher.registerHandler<network::EntitySpawned>(
      [this](const network::MessageOrigin&,
             const network::EntitySpawned& spawned) { keep(spawned); });
  dispatcher.registerHandler<network::EntityDestroyed>(
      [this](const network::MessageOrigin&,
             const network::EntityDestroyed& destroyed) { keep(destroyed); });
  dispatcher.registerHandler<network::GameEvent>(
      [this](const network::MessageOrigin&, const network::GameEvent& event) {
        if (gameEvents_.isFirstCopy(event.eventNumber)) {
          keep(event);
        }
      });
  dispatcher.registerHandler<network::GameEnded>(
      [this](const network::MessageOrigin&, const network::GameEnded& ended) {
        if (room_->gameStarted()) {
          keep(ended);
          room_->endGame();
        }
      });
}

void EntityMessageQueue::advance(engine::Duration elapsed) {
  gameEvents_.advance(elapsed);
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
  gameEvents_.forget();
}

void EntityMessageQueue::sessionHeld(
    [[maybe_unused]] engine::Duration elapsed) {}

void EntityMessageQueue::keep(EntityMessage message) {
  if (room_->gameStarted()) {
    messages_.push_back(std::move(message));
  }
}

}  // namespace rtype::client
