#pragma once

#include <functional>
#include <vector>
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The parts that follow the session, told together of what happens to
 * it, in the order they were given. */
class SessionObservers {
 public:
  /** @param observers Every observer must outlive this object. */
  explicit SessionObservers(
      std::vector<std::reference_wrapper<ISessionObserver>> observers);

  void opened(const network::LobbyPlayer& ownPlayer, ISessionOutbox& outbox);
  void closed();
  void held(engine::Duration elapsed);

 private:
  std::vector<std::reference_wrapper<ISessionObserver>> observers_;
};

}  // namespace rtype::client
