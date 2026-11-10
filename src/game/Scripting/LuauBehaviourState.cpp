#include "LuauBehaviourState.hpp"

namespace rtype::game {

void LuauBehaviourState::stopScript() {
  subscriptions.clear();
  thread.reset();
  waiting = false;
  heldIntent.stop();
}

}  // namespace rtype::game
