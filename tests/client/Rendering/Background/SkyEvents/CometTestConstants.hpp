#pragma once

#include "BackgroundConstants.hpp"
#include "SkyEventConstants.hpp"

/** @brief A second of the sky event clock, and the frames of FRAME it
 * holds: long enough for the gaps of a tail to run eight steps. */
constexpr float ONE_EVENT_SECOND = 1.0F;
constexpr int FRAMES_OF_A_SECOND = 100;

/** @brief How far a comet travels from where it enters to where it is over, in
 * background pixels. */
constexpr float COMET_CROSSING = rtype::client::BACKGROUND_WIDTH +
                                 rtype::client::COMET_ENTRY_MARGIN +
                                 rtype::client::COMET_OFF_SKY_MARGIN;
