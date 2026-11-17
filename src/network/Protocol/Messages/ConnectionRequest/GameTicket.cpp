#include "GameTicket.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "Hashing.hpp"
#include "NetworkConstants.hpp"

namespace rtype::network {

namespace {

/** @returns The value of the lowercase hexadecimal digit @p digit, nothing
 * for any other character. */
std::optional<std::size_t> digitValueOf(char digit) {
  const std::size_t value = LOWERCASE_HEX_DIGITS.find(digit);
  if (value == std::string_view::npos) {
    return std::nullopt;
  }
  return value;
}

}  // namespace

bool isGuestTicket(const GameTicket& ticket) { return ticket == GUEST_TICKET; }

std::optional<std::string> gameTicketText(const GameTicket& ticket) {
  if (isGuestTicket(ticket)) {
    return std::nullopt;
  }
  return std::string{GAME_TICKET_PREFIX} + hexadecimalOf(ticket.bytes);
}

std::optional<GameTicket> gameTicketOfText(std::string_view text) {
  if (text.size() != GAME_TICKET_SIZE ||
      !text.starts_with(GAME_TICKET_PREFIX)) {
    return std::nullopt;
  }
  const std::string_view digits = text.substr(GAME_TICKET_PREFIX.size());
  GameTicket ticket{};
  for (std::size_t index = 0; index < ticket.bytes.size(); ++index) {
    const std::optional<std::size_t> high =
        digitValueOf(digits[index * HEX_DIGITS_PER_BYTE]);
    const std::optional<std::size_t> low =
        digitValueOf(digits[(index * HEX_DIGITS_PER_BYTE) + 1]);
    if (!high.has_value() || !low.has_value()) {
      return std::nullopt;
    }
    ticket.bytes.at(index) =
        static_cast<std::byte>((*high * LOWERCASE_HEX_DIGITS.size()) + *low);
  }
  return ticket;
}

}  // namespace rtype::network
