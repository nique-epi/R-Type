#include "GameTicketText.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include "GameTicketTextConstants.hpp"
#include "Hashing.hpp"
#include "HashingConstants.hpp"
#include "NetworkConstants.hpp"

namespace rtype::master {

static_assert(GAME_TICKET_FINGERPRINT_CONTEXT.size() ==
              network::HASH_CONTEXT_LENGTH);

std::string newGameTicket() {
  std::array<std::byte, network::GAME_TICKET_BYTES> ticket{};
  network::fillWithRandomBytes(ticket);
  return std::string{network::GAME_TICKET_PREFIX} +
         network::hexadecimalOf(ticket);
}

std::string gameTicketFingerprintOf(std::string_view ticket) {
  return network::hexadecimalOf(
      network::hashOf(std::as_bytes(std::span{ticket.data(), ticket.size()}),
                      GAME_TICKET_FINGERPRINT_CONTEXT));
}

}  // namespace rtype::master
