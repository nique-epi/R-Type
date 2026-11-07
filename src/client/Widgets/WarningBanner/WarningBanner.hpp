#pragma once

#include <cstddef>
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How much of the WARNING screen shows, in pixels: how far the band
 * opened on each side of its middle, how far the tapes still are from their
 * place, how far their stripes scrolled, how many letters of the word and of
 * the line under it are written, and how far the bands of the word are torn
 * sideways. */
struct WarningBannerLook {
  int bandHalfHeight{};
  int tapeShift{};
  int stripeScroll{};
  std::size_t wordLetters{};
  std::size_t subtitleLetters{};
  int tear{};

  bool operator==(const WarningBannerLook&) const = default;
};

/**
 * @returns The WARNING screen at that time since the Arriving status of the
 * boss: nothing before WARNING_START nor from WARNING_END. The band opens
 * over WARNING_BAND_OPENING and the tapes slide in meanwhile; the word is
 * typed from WARNING_START, then the line under it; both leave when the band
 * starts closing, WARNING_BAND_CLOSING before the end. With
 * FlashPreference::Reduced, the word neither tears nor blinks.
 */
[[nodiscard]] WarningBannerLook warningBannerAt(engine::Duration sinceArrival,
                                                FlashPreference flashes);

/**
 * @brief Paints the screen over the playfield, never on the status band: the
 * band of WARNING_BAND_COLOR, a tape above and one under it, the word in its
 * fire bands over its shadow, outlined in INK and darkened by scanlines,
 * placed where the whole word goes so a new letter never moves the others,
 * and the line under it. Nothing for an empty look.
 */
void paintWarningBanner(IInterfaceCanvas& canvas,
                        const WarningBannerLook& look);

}  // namespace rtype::client
