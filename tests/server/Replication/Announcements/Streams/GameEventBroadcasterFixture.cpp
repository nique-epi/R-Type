#include "GameEventBroadcasterFixture.hpp"
#include <cstddef>
#include <memory>
#include "EncodedMessage.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameEventBroadcasterParts.hpp"

namespace rtype::server::testing {

GameEventBroadcasterFixture::GameEventBroadcasterFixture()
    : broadcaster_(
          std::make_unique<GameEventBroadcaster>(GameEventBroadcasterParts{
              .events = events_,
              .sendToEveryPlayer =
                  [this](const network::EncodedMessage& message) {
                    sentOnce_.push_back(message);
                  },
              .guaranteeToEveryPlayer =
                  [this](const network::EncodedMessage& message) {
                    guaranteed_.push_back(message);
                  }})) {}

void GameEventBroadcasterFixture::dispatch() { events_.dispatch(); }

void GameEventBroadcasterFixture::destroyBroadcaster() { broadcaster_.reset(); }

std::size_t GameEventBroadcasterFixture::messageCount() const {
  return sentOnce_.size() + guaranteed_.size();
}

}  // namespace rtype::server::testing
