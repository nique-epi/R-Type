#include "OptionsDescriptions.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "BoundAction.hpp"
#include "DisplayMode.hpp"
#include "Dropdown.hpp"
#include "KeySlot.hpp"
#include "OptionsScreenConstants.hpp"
#include "SettingsConstants.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeSelection.hpp"
#include "WindowSizeText.hpp"

namespace rtype::client {

namespace {

/** @returns A description naming a window size, then saying why. */
std::string sizeDescription(std::size_t index, std::string_view reason) {
  std::string text(DESCRIPTION_START);
  text += windowSizeText(WINDOW_SIZES[index]);
  text += reason;
  return text;
}

}  // namespace

std::string volumeDescription(int defaultVolume, bool muted) {
  if (muted) {
    return MUTED_VOLUME_DESCRIPTION;
  }
  std::string text(VOLUME_DESCRIPTION_STEP);
  text += std::to_string(VOLUME_STEP);
  text += VOLUME_DESCRIPTION_DEFAULT;
  text += std::to_string(defaultVolume);
  text += VOLUME_DESCRIPTION_END;
  return text;
}

std::string keyDescription(const KeySlot& slot,
                           std::optional<BoundAction> sharing) {
  if (slot.isListening()) {
    return slot.lastKeyRefused() ? REFUSED_KEY_DESCRIPTION
                                 : LISTENING_DESCRIPTION;
  }
  if (sharing.has_value()) {
    std::string text(CONFLICT_DESCRIPTION_START);
    text += BOUND_ACTION_LABELS[static_cast<std::size_t>(*sharing)];
    text += CONFLICT_DESCRIPTION_END;
    return text;
  }
  return KEY_DESCRIPTION;
}

const char* displayModeDescription(const Dropdown& modes, DisplayMode mode) {
  if (modes.isOpen()) {
    return OPEN_LIST_DESCRIPTION;
  }
  if (mode == DisplayMode::Borderless) {
    return BORDERLESS_DESCRIPTION;
  }
  return DISPLAY_MODE_DESCRIPTION;
}

std::string windowSizeDescription(const Dropdown& sizes,
                                  const WindowSizeSelection& selection) {
  if (sizes.isOpen()) {
    const std::optional<std::size_t> pointed = sizes.pointedChoice();
    if (pointed.has_value() && !selection.isAvailable(*pointed)) {
      return sizeDescription(*pointed, POINTED_TOO_LARGE_END);
    }
    return OPEN_LIST_DESCRIPTION;
  }
  const std::size_t larger = selection.selectedIndex() + 1;
  if (larger < WINDOW_SIZES.size() && !selection.isAvailable(larger)) {
    return sizeDescription(larger, LARGER_TOO_LARGE_END);
  }
  return WINDOW_SIZE_DESCRIPTION;
}

}  // namespace rtype::client
