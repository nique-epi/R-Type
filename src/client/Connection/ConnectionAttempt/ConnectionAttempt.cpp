#include "ConnectionAttempt.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include "AttemptProgress.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionRequest.hpp"
#include "ISessionOutbox.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void ConnectionAttempt::begin(std::string_view nickname) {
  nickname_ = nickname;
  cookie_ = 0;
  sinceBegin_ = engine::Duration::zero();
}

void ConnectionAttempt::sendRequest(ISessionOutbox& outbox) {
  sendMessage(outbox, network::ConnectionRequest{.cookie = cookie_,
                                                 .nickname = nickname_});
  sinceRequest_ = engine::Duration::zero();
}

void ConnectionAttempt::answerChallenge(std::uint64_t cookie,
                                        ISessionOutbox& outbox) {
  cookie_ = cookie;
  sendRequest(outbox);
}

AttemptProgress ConnectionAttempt::advance(engine::Duration elapsed,
                                           ISessionOutbox& outbox) {
  sinceBegin_ += elapsed;
  if (sinceBegin_ >= CONNECTION_ATTEMPT_TIMEOUT) {
    return AttemptProgress::TimedOut;
  }
  sinceRequest_ += elapsed;
  if (sinceRequest_ >= CONNECTION_REQUEST_INTERVAL) {
    sendRequest(outbox);
  }
  return AttemptProgress::Waiting;
}

const std::string& ConnectionAttempt::nickname() const { return nickname_; }

}  // namespace rtype::client
