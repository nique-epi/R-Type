#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::network {

/**
 * @returns What a traffic file calls the messages at @p slot of
 * DirectionTraffic::messages: the name of the message, as the protocol page
 * writes it, or UNKNOWN_MESSAGE_TEXT.
 */
[[nodiscard]] std::string_view messageTypeText(std::size_t slot);

}  // namespace rtype::network
