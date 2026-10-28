#include "SessionLossWatch.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionStatus.hpp"

namespace rtype::client {

bool SessionLossWatch::endsTheSession(const ConnectionStatus& status) {
  const bool ended = status.phase == ConnectionPhase::Lost ||
                     status.phase == ConnectionPhase::Closed;
  const bool changed = status.phase != seen_;
  seen_ = status.phase;
  return ended && changed;
}

}  // namespace rtype::client
