#pragma once

#include "IncomingHit.hpp"

namespace rtype::game {

/**
 * @brief What a behaviour remembers of one body between two ticks, the way it
 * judges the hits the body takes included. The Pilot of the body owns it, so
 * it ends with the body, whichever way the body is destroyed.
 */
class IBehaviourState {
 public:
  virtual ~IBehaviourState() = default;

  /** @returns Whether @p hit applies to the body, as its behaviour decides
   * before the hit is announced: true unless the behaviour cancels it. */
  [[nodiscard]] virtual bool letsHit(const IncomingHit& hit) = 0;
};

}  // namespace rtype::game
