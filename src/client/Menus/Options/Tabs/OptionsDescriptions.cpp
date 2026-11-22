#include "OptionsDescriptions.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "BoundAction.hpp"
#include "DisplayMode.hpp"
#include "Dropdown.hpp"
#include "KeySlot.hpp"
#include "Language.hpp"
#include "OptionsScreenConstants.hpp"
#include "SettingsConstants.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"
#include "Translation.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeSelection.hpp"
#include "WindowSizeText.hpp"

namespace rtype::client {

namespace {

/** @returns A description naming a window size, then saying why, in
 * @p language. */
std::string sizeDescription(std::size_t index, const Translation& reason,
                            Language language) {
  std::string text(DESCRIPTION_START);
  text += windowSizeText(WINDOW_SIZES[index]);
  text += reason.in(language);
  return text;
}

}  // namespace

std::string volumeDescription(int defaultVolume, bool muted,
                              Language language) {
  if (muted) {
    return std::string(MUTED_VOLUME_DESCRIPTION.in(language));
  }
  const std::string step = std::to_string(VOLUME_STEP);
  const std::string byDefault = std::to_string(defaultVolume);
  return withMark(withMark(std::string(VOLUME_DESCRIPTION.in(language)),
                           TextMark{.mark = STEP_MARK, .value = step}),
                  TextMark{.mark = DEFAULT_MARK, .value = byDefault});
}

std::string keyDescription(const KeySlot& slot,
                           std::optional<BoundAction> sharing,
                           Language language) {
  if (slot.isListening()) {
    return std::string(slot.lastKeyRefused()
                           ? REFUSED_KEY_DESCRIPTION.in(language)
                           : LISTENING_DESCRIPTION.in(language));
  }
  if (sharing.has_value()) {
    return withMark(
        std::string(CONFLICT_DESCRIPTION.in(language)),
        TextMark{
            .mark = ACTION_MARK,
            .value = BOUND_ACTION_LABELS[static_cast<std::size_t>(*sharing)].in(
                language)});
  }
  return std::string(KEY_DESCRIPTION.in(language));
}

std::string displayModeDescription(const Dropdown& modes, DisplayMode mode,
                                   Language language) {
  if (modes.isOpen()) {
    return std::string(OPEN_LIST_DESCRIPTION.in(language));
  }
  if (mode == DisplayMode::Borderless) {
    return std::string(BORDERLESS_DESCRIPTION.in(language));
  }
  return std::string(DISPLAY_MODE_DESCRIPTION.in(language));
}

std::string windowSizeDescription(const Dropdown& sizes,
                                  const WindowSizeSelection& selection,
                                  Language language) {
  if (sizes.isOpen()) {
    const std::optional<std::size_t> pointed = sizes.pointedChoice();
    if (pointed.has_value() && !selection.isAvailable(*pointed)) {
      return sizeDescription(*pointed, POINTED_TOO_LARGE_END, language);
    }
    return std::string(OPEN_LIST_DESCRIPTION.in(language));
  }
  const std::size_t larger = selection.selectedIndex() + 1;
  if (larger < WINDOW_SIZES.size() && !selection.isAvailable(larger)) {
    return sizeDescription(larger, LARGER_TOO_LARGE_END, language);
  }
  return std::string(WINDOW_SIZE_DESCRIPTION.in(language));
}

}  // namespace rtype::client
