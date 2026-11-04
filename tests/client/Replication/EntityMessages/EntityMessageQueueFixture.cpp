#include "EntityMessageQueueFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatagramCodec.hpp"
#include "DeltaWorldState.hpp"
#include "DeltaWorldStateCodec.hpp"
#include "MessageType.hpp"

namespace rtype::client::testing {

void EntityMessageQueueFixture::deliverDeltaInSession(
    const network::DeltaWorldState& delta, std::uint16_t sequence) {
  const std::vector<std::vector<std::byte>> payloads =
      network::DeltaWorldStateCodec::encode(delta);
  deliverBytes(network::DatagramCodec::encode(
      sessionHeader(sequence), network::MessageType::DeltaWorldState,
      payloads.front()));
}

}  // namespace rtype::client::testing
