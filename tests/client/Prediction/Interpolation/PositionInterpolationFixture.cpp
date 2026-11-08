#include "PositionInterpolationFixture.hpp"
#include <cstdint>
#include "InterpolationTestConstants.hpp"
#include "PositionHistory.hpp"
#include "PositionInterpolation.hpp"
#include "PositionSample.hpp"

namespace rtype::client::testing {

PositionInterpolationFixture::PositionInterpolationFixture()
    : entity_(entities_.create()) {
  components_.add(entity_, WHERE_THE_SERVER_PUT_IT);
  components_.add(entity_, PositionHistory{});
}

void PositionInterpolationFixture::record(PositionSample sample) {
  components_.get<PositionHistory>(entity_)->record(sample);
}

void PositionInterpolationFixture::receiveStateOfTick(std::uint32_t tick) {
  clock_.observe(tick);
}

void PositionInterpolationFixture::place() {
  placeInterpolatedPositions(clock_, components_);
}

game::Position PositionInterpolationFixture::position() {
  return *components_.get<game::Position>(entity_);
}

}  // namespace rtype::client::testing
