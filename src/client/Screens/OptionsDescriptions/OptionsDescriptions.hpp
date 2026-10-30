#pragma once

#include <optional>
#include <string>
#include "BoundAction.hpp"
#include "DisplayMode.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

class Dropdown;
class KeySlot;

/** @returns The description of a volume: its step and its default, or why its
 * slider is dimmed while the sound is muted. */
[[nodiscard]] std::string volumeDescription(int defaultVolume, bool muted);

/** @returns The description of a key slot: what it awaits while it listens,
 * the action @p sharing its key if there is one, else what the slot does. */
[[nodiscard]] std::string keyDescription(const KeySlot& slot,
                                         std::optional<BoundAction> sharing);

/** @returns The description of the list of display modes: its keys while it is
 * open, what borderless does while the window is borderless. */
[[nodiscard]] const char* displayModeDescription(const Dropdown& modes,
                                                 DisplayMode mode);

/** @returns The description of the list of sizes: its keys while it is open,
 * the size too large for the screen it points at or sits above, else what the
 * list does. */
[[nodiscard]] std::string windowSizeDescription(
    const Dropdown& sizes, const WindowSizeSelection& selection);

}  // namespace rtype::client
