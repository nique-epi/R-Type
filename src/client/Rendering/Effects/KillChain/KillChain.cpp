#include "KillChain.hpp"
#include "EffectConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

bool KillChain::noteKill() {
  if (sinceKill_ >= KILL_CHAIN_WINDOW) {
    length_ = 0;
  }
  ++length_;
  sinceKill_ = engine::Duration::zero();
  return length_ % BIG_CATCH_EVERY == 0;
}

void KillChain::advance(engine::Duration elapsed) { sinceKill_ += elapsed; }

}  // namespace rtype::client
