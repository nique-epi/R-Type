#include "GameLoadedReminder.hpp"
#include "ConnectionConstants.hpp"
#include "GameLoaded.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void GameLoadedReminder::begin() {
  started_ = true;
  sinceGameLoaded_ = GAME_LOADED_INTERVAL;
}

void GameLoadedReminder::worldStateArrived() { worldStateSeen_ = true; }

void GameLoadedReminder::end() {
  started_ = false;
  worldStateSeen_ = false;
}

void GameLoadedReminder::sendWhenDue(ISessionOutbox& outbox) {
  if (started_ && !worldStateSeen_ &&
      sinceGameLoaded_ >= GAME_LOADED_INTERVAL) {
    sendMessage(outbox, network::GameLoaded{});
    sinceGameLoaded_ = engine::Duration::zero();
  }
}

void GameLoadedReminder::sessionOpened(
    [[maybe_unused]] const network::LobbyPlayer& ownPlayer,
    [[maybe_unused]] ISessionOutbox& outbox) {}

void GameLoadedReminder::sessionClosed() { end(); }

void GameLoadedReminder::sessionHeld(engine::Duration elapsed) {
  sinceGameLoaded_ += elapsed;
}

}  // namespace rtype::client
