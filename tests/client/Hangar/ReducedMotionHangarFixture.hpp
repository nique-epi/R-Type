#pragma once

#include "HangarScreenFixture.hpp"

namespace rtype::client::testing {

/** @brief The hangar of HangarScreenFixture, built while the system asks for
 * less motion. */
class ReducedMotionHangarFixture : public HangarScreenFixture {
 protected:
  ReducedMotionHangarFixture();
};

}  // namespace rtype::client::testing
