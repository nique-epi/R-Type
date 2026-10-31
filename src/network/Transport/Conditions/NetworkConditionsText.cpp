#include "NetworkConditionsText.hpp"
#include <string>
#include "NetworkConditions.hpp"

namespace rtype::network {

std::string conditionsText(const NetworkConditions& conditions) {
  return "ping +" + std::to_string(conditions.lag.count()) + " ms, up to " +
         std::to_string(conditions.jitter.count()) + " ms more, " +
         std::to_string(conditions.lossPercent) + " % lost, " +
         std::to_string(conditions.duplicatePercent) + " % duplicated, " +
         std::to_string(conditions.reorderPercent) + " % reordered";
}

}  // namespace rtype::network
