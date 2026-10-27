#pragma once

#include "BackgroundConstants.hpp"
#include "SkyEventConstants.hpp"

/** @brief How far an asteroid travels from where it enters to where it is over,
 * in background pixels. */
constexpr float ASTEROID_CROSSING =
    rtype::client::BACKGROUND_WIDTH +
    (2.0F * rtype::client::ASTEROID_OFF_SKY_MARGIN);
