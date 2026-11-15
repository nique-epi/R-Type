#pragma once

#include "FlashPreference.hpp"
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"

/** @brief When and how a test paints the band: the time since the start of
 * the game, whether flashes are reduced, and the text size. */
struct BandMoment {
  rtype::engine::Duration sinceStart{};
  rtype::client::FlashPreference flashes = rtype::client::FlashPreference::Full;
  rtype::client::TextMetrics metrics{};
};
