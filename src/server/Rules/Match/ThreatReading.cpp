#include "ThreatReading.hpp"
#include "IThreatLevel.hpp"

namespace rtype::server {

int threatPercentOf(const IThreatLevel* threat) {
  return threat == nullptr ? 0 : threat->level().percent;
}

}  // namespace rtype::server
