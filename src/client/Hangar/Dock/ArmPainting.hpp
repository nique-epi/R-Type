#pragma once

#include "ArmMount.hpp"
#include "ArmPose.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints an arm bolted at @p mount whose jaws close on
 * @p pose.target: its base, its piston, its two links bent at an elbow that
 * reaches the target, its joints, its drum and its three jaws. */
void paintArm(IInterfaceCanvas& canvas, ArmMount mount, ArmPose pose);

}  // namespace rtype::client
