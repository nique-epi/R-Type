#include "SessionObservers.hpp"
#include <functional>
#include <utility>
#include <vector>
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

SessionObservers::SessionObservers(
    std::vector<std::reference_wrapper<ISessionObserver>> observers)
    : observers_(std::move(observers)) {}

void SessionObservers::opened(const network::LobbyPlayer& ownPlayer,
                              ISessionOutbox& outbox) {
  for (ISessionObserver& observer : observers_) {
    observer.sessionOpened(ownPlayer, outbox);
  }
}

void SessionObservers::closed() {
  for (ISessionObserver& observer : observers_) {
    observer.sessionClosed();
  }
}

void SessionObservers::held(engine::Duration elapsed) {
  for (ISessionObserver& observer : observers_) {
    observer.sessionHeld(elapsed);
  }
}

}  // namespace rtype::client
