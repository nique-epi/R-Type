#pragma once

#include "DeviceLinkAsks.hpp"
#include "DeviceLinkReading.hpp"
#include "LatestValue.hpp"

namespace rtype::client {

/** @brief What the frame loop and the thread of a MasterDeviceLinkFlow share:
 * the asks, newest first, and where the newest link stands. */
struct DeviceLinkFlowState {
  concurrency::LatestValue<DeviceLinkAsks> asks;
  concurrency::LatestValue<DeviceLinkReading> reading;
};

}  // namespace rtype::client
