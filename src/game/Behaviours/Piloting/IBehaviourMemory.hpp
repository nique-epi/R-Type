#pragma once

namespace rtype::game {

/**
 * @brief What a behaviour remembers of one body between two ticks. The Pilot
 * of the body owns it, so it ends with the body, whichever way the body is
 * destroyed.
 */
class IBehaviourMemory {
 public:
  virtual ~IBehaviourMemory() = default;
};

}  // namespace rtype::game
