#include "MessageTypeText.hpp"
#include <cstddef>
#include <string_view>
#include "MessageSlotConstants.hpp"
#include "TrafficRecordingConstants.hpp"

namespace rtype::network {

std::string_view messageTypeText(std::size_t slot) {
  if (slot < FIRST_NAMED_MESSAGE_TYPE || slot >= MESSAGE_SLOT_COUNT) {
    return UNKNOWN_MESSAGE_TEXT;
  }
  return MESSAGE_TYPE_NAMES[slot - FIRST_NAMED_MESSAGE_TYPE];
}

}  // namespace rtype::network
