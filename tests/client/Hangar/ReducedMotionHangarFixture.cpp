#include "ReducedMotionHangarFixture.hpp"
#include "HangarScreenFixture.hpp"
#include "MotionPreference.hpp"

namespace rtype::client::testing {

ReducedMotionHangarFixture::ReducedMotionHangarFixture()
    : HangarScreenFixture(MotionPreference::Reduced) {}

}  // namespace rtype::client::testing
