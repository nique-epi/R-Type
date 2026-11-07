#include "ArrivalRevealFixture.hpp"
#include <cstdint>
#include <vector>
#include "ArrivalReveal.hpp"
#include "ArrivalRevealTestConstants.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorFixtures.hpp"
#include "Position.hpp"
#include "PositionInterpolation.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

void ArrivalRevealFixture::fireTheMissileOf(std::uint8_t typeNumber) {
  replicator_.apply(
      spawned(REVEALED_MISSILE_ID, typeNumber, MISSILE_AT_THE_MUZZLE));
  receive(stateListing(
      FIRST_MISSILE_STATE_TICK,
      {listed(REVEALED_MISSILE_ID, typeNumber, MISSILE_AT_THE_MUZZLE)}));
  receive(stateListing(
      SECOND_MISSILE_STATE_TICK,
      {listed(REVEALED_MISSILE_ID, typeNumber, MISSILE_THREE_TICKS_LATER)}));
}

void ArrivalRevealFixture::receive(const network::WorldState& state) {
  replicator_.apply(state);
  clock_.observe(state.tick);
}

void ArrivalRevealFixture::showOneMoreFrame() {
  clock_.advance(engine::SIMULATION_TICK_DURATION);
  placeInterpolatedPositions(clock_, components_);
  revealArrivedEntities(clock_, components_, feedback_);
}

bool ArrivalRevealFixture::missileIsDrawn() {
  return !drawnEntities(components_).empty();
}

game::Position ArrivalRevealFixture::missilePosition() {
  const std::vector<DrawnEntity> drawn = drawnEntities(components_);
  if (drawn.empty()) {
    return NO_MISSILE_POSITION;
  }
  return game::Position{.x = drawn.front().x, .y = drawn.front().y};
}

const RecordingSoundPlayer& ArrivalRevealFixture::sounds() const {
  return sounds_;
}

}  // namespace rtype::client::testing
