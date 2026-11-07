#pragma once

#include <optional>
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief What the warning of the charge shows: whether it shows, the row of
 * the interface picture its lane runs along, whether its text is lit, and
 * how far its dashes scrolled. */
struct WatchOutLook {
  bool shown{};
  int row{};
  bool lit{};
  int scroll{};

  bool operator==(const WatchOutLook&) const = default;
};

/** @brief Where the boss charges and how long ago its charge was signalled:
 * the row of its lane on the interface picture, and the time since the
 * signal, nothing before any. */
struct WatchOutCue {
  int row{};
  std::optional<engine::Duration> sinceSignal;
};

/** @returns The warning of the charge @p cue gives, with @p flashes: shown
 * for WATCH_OUT_TIME from the signal, its text blinking every
 * WATCH_OUT_BLINK_STEP unless flashes are reduced; nothing otherwise. */
[[nodiscard]] WatchOutLook watchOutAt(const WatchOutCue& cue,
                                      FlashPreference flashes);

/** @brief Paints @p look on @p canvas: the dashed lane across the playfield
 * on its row, and WATCH_OUT_TEXT above its left end while lit. Nothing when
 * it is not shown. */
void paintWatchOut(IInterfaceCanvas& canvas, const WatchOutLook& look);

}  // namespace rtype::client
