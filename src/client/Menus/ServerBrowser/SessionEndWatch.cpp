#include "SessionEndWatch.hpp"
#include "ConnectionPhase.hpp"
#include "IServerConnection.hpp"
#include "ServerMapScreenConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

SessionEndWatch::SessionEndWatch(const IServerConnection& connection)
    : connection_(&connection), seen_(connection.status().phase) {}

bool SessionEndWatch::recheckDue(engine::Duration elapsed) {
  const ConnectionPhase phase = connection_->status().phase;
  if (phase == ConnectionPhase::Idle && seen_ != ConnectionPhase::Idle) {
    sinceEnd_ = engine::Duration::zero();
  }
  seen_ = phase;
  if (!sinceEnd_.has_value()) {
    return false;
  }
  *sinceEnd_ += elapsed;
  if (*sinceEnd_ < GAMES_RECHECK_DELAY) {
    return false;
  }
  sinceEnd_.reset();
  return true;
}

}  // namespace rtype::client
