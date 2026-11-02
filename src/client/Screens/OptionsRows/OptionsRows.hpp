#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "BoundAction.hpp"
#include "ISizableWindow.hpp"
#include "KeyNames.hpp"
#include "RowControl.hpp"
#include "Setting.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

class Dropdown;
class KeySlot;

/** @returns The texts of a list of labels. */
template <std::size_t Count>
[[nodiscard]] std::vector<std::string> textsOf(
    const std::array<std::string_view, Count>& labels) {
  return {labels.begin(), labels.end()};
}

/** @returns The slider of a volume, dimmed while the sound is muted; it reads
 * and changes @p volume through @p service. */
[[nodiscard]] std::unique_ptr<RowControl> volumeSlider(
    SettingsService& service, const Setting<int>& volume);

/** @returns The switch of a setting; it reads and changes @p setting through
 * @p service. */
[[nodiscard]] std::unique_ptr<RowControl> settingSwitch(
    SettingsService& service, const Setting<bool>& setting);

/** @returns The selector of the color mode, which reads and changes it through
 * @p service. */
[[nodiscard]] std::unique_ptr<RowControl> colorModeSelector(
    SettingsService& service);

/** @returns The slot of the key of @p action, which reads and binds it through
 * @p service and names keys with @p naming. */
[[nodiscard]] std::unique_ptr<KeySlot> keySlot(SettingsService& service,
                                               const KeyNaming& naming,
                                               BoundAction action);

/** @returns The list of DISPLAY_MODES, showing the mode of the window. A mode
 * picked goes to the window and, once the window takes it, to @p service. */
[[nodiscard]] std::unique_ptr<Dropdown> displayModeList(
    ISizableWindow& window, SettingsService& service);

/** @returns The list of WINDOW_SIZES, showing the size of the window, or the
 * size its display mode imposes. A size picked goes to the window and, once
 * the window takes it, to @p service. */
[[nodiscard]] std::unique_ptr<Dropdown> windowSizeList(
    ISizableWindow& window, SettingsService& service);

}  // namespace rtype::client
