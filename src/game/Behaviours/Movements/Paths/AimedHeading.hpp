#pragma once

#include "Direction.hpp"
#include "IBehaviourState.hpp"
#include "IncomingHit.hpp"

namespace rtype::game {

/**
 * @brief What the aimed behaviour remembers of one body: the heading it took
 * on its first tick, which it keeps for the rest of its flight. It cancels no
 * hit.
 */
class AimedHeading final : public IBehaviourState {
 public:
  explicit AimedHeading(Direction heading);

  /** @returns The heading taken on the first tick of the body. */
  [[nodiscard]] Direction heading() const;
  /** @returns true: an aimed body takes every hit. */
  [[nodiscard]] bool letsHit(const IncomingHit& hit) override;

 private:
  Direction heading_;
};

}  // namespace rtype::game
