#include "MessageTypeText.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "StatisticsConstants.hpp"

namespace rtype::network {

std::string messageTypeText(std::uint8_t typeByte) {
  const std::size_t type = typeByte;
  if (type < FIRST_NAMED_MESSAGE_TYPE ||
      type >= FIRST_NAMED_MESSAGE_TYPE + MESSAGE_TYPE_NAMES.size()) {
    return std::string{UNKNOWN_MESSAGE_TYPE_PREFIX} + std::to_string(type);
  }
  return std::string{MESSAGE_TYPE_NAMES[type - FIRST_NAMED_MESSAGE_TYPE]};
}

}  // namespace rtype::network
