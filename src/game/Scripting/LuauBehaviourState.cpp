#include "LuauBehaviourState.hpp"
#include <string>
#include "EngineException.hpp"
#include "GameScriptingConstants.hpp"
#include "IncomingHit.hpp"

namespace rtype::game {

bool LuauBehaviourState::letsHit(const IncomingHit& hit) {
  if (!preHit.has_value()) {
    return true;
  }
  try {
    return preHit->lets(hit);
  } catch (const engine::ScriptException& error) {
    preHitFailure = std::string{PRE_HIT_FAILURE_PREFIX} + error.what();
    preHit.reset();
    return true;
  }
}

}  // namespace rtype::game
