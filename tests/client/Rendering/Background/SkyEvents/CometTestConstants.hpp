#pragma once

#include <cstddef>
#include "BackgroundConstants.hpp"
#include "SkyEventConstants.hpp"

/** @brief A second of the sky event clock, and the frames of FRAME it
 * holds: long enough for the gaps of a tail to run eight steps. */
constexpr float ONE_EVENT_SECOND = 1.0F;
constexpr int FRAMES_OF_A_SECOND = 100;

/** @brief The plots of a comet in a steady sky: the 27 pixels of its tail
 * less the 8 gaps that stand still past its first 5 pixels, 2 more for each
 * of its 4 thick pixels, and the 4 pixels of its head. */
constexpr std::size_t PLOTS_OF_A_STEADY_COMET = 31;

/** @brief How far a comet travels from where it enters to where it is over, in
 * background pixels. */
constexpr float COMET_CROSSING = rtype::client::BACKGROUND_WIDTH +
                                 rtype::client::COMET_ENTRY_MARGIN +
                                 rtype::client::COMET_OFF_SKY_MARGIN;
