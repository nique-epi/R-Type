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

void DeltaConvergenceFixture::receiveSentDatagrams() {
  for (; handled_ < sentDatagrams().size(); ++handled_) {
    if (isLost(handled_)) {
      continue;
    }
    const network::OutgoingDatagram& datagram = sentDatagrams().at(handled_);
    ++report_.delivered;
    static_cast<void>(received_.accept(
        network::DatagramCodec::decodeHeader(datagram.payload).sequence));
    const auto part = onlyMessageIn<network::DeltaWorldState>(datagram);
    const std::optional<network::WorldState> state = assembler_.add(part);
    if (state.has_value()) {
      ++report_.rebuilt;
      report_.mismatching += state->entities == worldAt(state->tick) ? 0 : 1;
      ++(part.baseDistance == 0 ? report_.rebuiltInFull
                                : report_.rebuiltFromDeltas);
    }
  }
}

void DeltaConvergenceFixture::moveHalfOfTheEnemies(std::uint32_t tick) {
  for (std::size_t index = 0; index < HALF_THE_ENTITIES; ++index) {
    move(enemies_.at(index),
         game::Position{.x = static_cast<float>(index + 1 + tick), .y = 0.0F});
  }
}

void DeltaConvergenceFixture::acknowledgeAcceptedBy(ConnectedPlayer& player) {
  network::PacketHeader acknowledgement;
  received_.acknowledgeIn(acknowledgement);
  player.sentStates.acknowledge(acknowledgement);
}

ConvergenceReport DeltaConvergenceFixture::playOverALossyNetwork() {
  for (std::uint32_t id = 1; id <= MEASURED_ENTITIES; ++id) {
    enemies_.push_back(
        addEnemy(id, game::Position{.x = static_cast<float>(id), .y = 0.0F}));
  }
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  for (std::uint32_t tick = 1; tick <= CONVERGENCE_TICKS; ++tick) {
    runTicks(1);
    receiveSentDatagrams();
    moveHalfOfTheEnemies(tick);
    acknowledgeAcceptedBy(player);
  }
  return report_;
}

}  // namespace rtype::server::testing
