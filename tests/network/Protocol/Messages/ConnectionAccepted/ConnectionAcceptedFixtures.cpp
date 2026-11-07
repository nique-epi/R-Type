#include "ConnectionAcceptedFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptedTestConstants.hpp"
#include "ContentAnnouncement.hpp"
#include "ContentFingerprint.hpp"

namespace rtype::network::testing {

ConnectionAccepted acceptanceOfAPackOf(std::uint32_t size) {
  ContentFingerprint fingerprint;
  for (std::size_t index = 0; index < fingerprint.bytes.size(); ++index) {
    fingerprint.bytes[index] = static_cast<std::byte>(index);
  }
  return ConnectionAccepted{
      .playerNumber = FIRST_PLAYER_BYTE,
      .content = ContentAnnouncement{
          .fingerprint = fingerprint, .size = size, .port = SOME_PACK_PORT}};
}

std::vector<std::byte> payloadOnTheProtocolPage() {
  std::vector<std::byte> payload{std::byte{FIRST_PLAYER_BYTE}};
  for (std::size_t index = 0; index < ContentFingerprint{}.bytes.size();
       ++index) {
    payload.push_back(static_cast<std::byte>(index));
  }
  for (const std::uint8_t value :
       {SIZE_FIRST_BYTE, SIZE_SECOND_BYTE, SIZE_THIRD_BYTE, SIZE_FOURTH_BYTE,
        PORT_FIRST_BYTE, PORT_SECOND_BYTE}) {
    payload.push_back(std::byte{value});
  }
  return payload;
}

}  // namespace rtype::network::testing
