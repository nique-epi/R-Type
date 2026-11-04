#include "DeltaConvergenceFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "DeltaConvergenceTestConstants.hpp"
#include "DeltaStateSenderTestConstants.hpp"
#include "DeltaWorldState.hpp"
#include "Entity.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "Position.hpp"
#include "ReceivedSequences.hpp"
#include "SentDatagrams.hpp"
#include "WorldState.hpp"
#include "WorldStateAssembler.hpp"

namespace rtype::server::testing {

namespace {

bool isLost(std::size_t datagramIndex) {
  const bool inTheOutage =
      datagramIndex >= OUTAGE_FIRST_DATAGRAM &&
      datagramIndex < OUTAGE_FIRST_DATAGRAM + OUTAGE_LENGTH;
  return inTheOutage || datagramIndex % LOSS_PERIOD == LOSS_PHASE;
}

/** @returns The enemies the world holds at @p tick: the first half moved
 * tick - 1 units from where they started. */
std::vector<network::EntityState> worldAt(std::uint32_t tick) {
  std::vector<network::EntityState> enemies;
  for (std::uint32_t id = 1; id <= MEASURED_ENTITIES; ++id) {
    const float moved =
        id <= HALF_THE_ENTITIES ? static_cast<float>(tick - 1) : 0.0F;
    enemies.push_back(
        network::EntityState{.entityId = id,
                             .entityType = network::WireEntityType::Bydo,
                             .x = static_cast<float>(id) + moved,
                             .y = 0.0F});
  }
  return enemies;
}

}  // namespace

ConvergenceReport DeltaConvergenceFixture::playOverALossyNetwork() {
  std::vector<engine::Entity> enemies;
  for (std::uint32_t id = 1; id <= MEASURED_ENTITIES; ++id) {
    enemies.push_back(
        addEnemy(id, game::Position{.x = static_cast<float>(id), .y = 0.0F}));
  }
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  client::WorldStateAssembler assembler;
  network::ReceivedSequences received;
  ConvergenceReport report{.delivered = 0,
                           .rebuilt = 0,
                           .mismatching = 0,
                           .rebuiltFromDeltas = 0,
                           .rebuiltInFull = 0};
  std::size_t handled = 0;
  for (std::uint32_t tick = 1; tick <= CONVERGENCE_TICKS; ++tick) {
    runTicks(1);
    for (; handled < sentDatagrams().size(); ++handled) {
      if (isLost(handled)) {
        continue;
      }
      const network::OutgoingDatagram& datagram = sentDatagrams().at(handled);
      ++report.delivered;
      static_cast<void>(received.accept(
          network::DatagramCodec::decodeHeader(datagram.payload).sequence));
      const network::DeltaWorldState part =
          onlyMessageIn<network::DeltaWorldState>(datagram);
      const std::optional<network::WorldState> state = assembler.add(part);
      if (state.has_value()) {
        ++report.rebuilt;
        report.mismatching += state->entities == worldAt(state->tick) ? 0 : 1;
        ++(part.baseDistance == 0 ? report.rebuiltInFull
                                  : report.rebuiltFromDeltas);
      }
    }
    for (std::size_t index = 0; index < HALF_THE_ENTITIES; ++index) {
      move(
          enemies.at(index),
          game::Position{.x = static_cast<float>(index + 1 + tick), .y = 0.0F});
    }
    network::PacketHeader acknowledgement;
    received.acknowledgeIn(acknowledgement);
    player.sentStates.acknowledge(acknowledgement);
  }
  return report;
}

}  // namespace rtype::server::testing
