#include "MessageTypeText.hpp"
#include <cstddef>
#include <string>
#include "StatisticsConstants.hpp"

namespace rtype::network {

std::string messageTypeText(std::size_t type) {
  if (type < FIRST_NAMED_MESSAGE_TYPE ||
      type >= FIRST_NAMED_MESSAGE_TYPE + MESSAGE_TYPE_NAMES.size()) {
    return std::string{UNKNOWN_MESSAGE_TYPE_PREFIX} + std::to_string(type);
  }
  return std::string{MESSAGE_TYPE_NAMES[type - FIRST_NAMED_MESSAGE_TYPE]};
}

}  // namespace rtype::network
