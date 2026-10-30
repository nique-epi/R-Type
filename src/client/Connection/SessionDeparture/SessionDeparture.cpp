#include "SessionDeparture.hpp"
#include <string>
#include "ConnectionConstants.hpp"
#include "Disconnection.hpp"
#include "ISessionOutbox.hpp"
#include "SessionOutbox.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

SessionDeparture::SessionDeparture()
    : logger_(std::string{CONNECTION_LOGGER_NAME}) {}

void SessionDeparture::leave(const SessionOutbox& session) {
  leaving_ = session;
  copiesLeft_ = DISCONNECTION_COPY_COUNT;
  sinceCopy_ = engine::Duration::zero();
  sendCopy();
  logger_.info("left ", session.server().address, " port ",
               session.server().port);
}

void SessionDeparture::update(engine::Duration elapsed) {
  if (!leaving_.has_value()) {
    return;
  }
  sinceCopy_ += elapsed;
  if (sinceCopy_ >= DISCONNECTION_COPY_INTERVAL) {
    sendCopy();
  }
}

bool SessionDeparture::isLeaving() const { return leaving_.has_value(); }

void SessionDeparture::sendCopy() {
  if (!leaving_.has_value()) {
    return;
  }
  sendMessage(*leaving_, network::Disconnection{});
  --copiesLeft_;
  sinceCopy_ = engine::Duration::zero();
  if (copiesLeft_ <= 0) {
    leaving_.reset();
  }
}

}  // namespace rtype::client
