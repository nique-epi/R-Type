#include "AimedHeading.hpp"
#include "Direction.hpp"
#include "IncomingHit.hpp"

namespace rtype::game {

AimedHeading::AimedHeading(Direction heading) : heading_(heading) {}

Direction AimedHeading::heading() const { return heading_; }

bool AimedHeading::letsHit([[maybe_unused]] const IncomingHit& hit) {
  return true;
}

}  // namespace rtype::game
