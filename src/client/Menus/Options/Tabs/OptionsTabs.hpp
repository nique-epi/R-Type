#pragma once

#include <cstddef>
#include <vector>
#include "ISizableWindow.hpp"
#include "Menu.hpp"
#include "OptionsRowHandles.hpp"
#include "OptionsTabsParts.hpp"
#include "SettingsService.hpp"
#include "TextSize.hpp"

namespace rtype::client {

class Dropdown;

/**
 * @brief The five tabs of the options, SOUND, CONTROLS, DISPLAY, ACCESS and
 * CHAT, each a panel of rows over BACK, and the one that is open.
 *
 * The rows read and change the settings through the SettingsService. The
 * descriptions follow the state of the rows: the keys of an open list, a key
 * awaited, a key two actions share, the sound muted, a size too large for the
 * screen, what borderless does.
 */
class OptionsTabs {
 public:
  explicit OptionsTabs(const OptionsTabsParts& parts);

  /** @brief Opens @p tab; ignores a tab there is not. */
  void openTab(std::size_t tab);

  [[nodiscard]] Menu& openMenu();
  [[nodiscard]] const Menu& openMenu() const;

  /** @returns The list of window sizes of the DISPLAY tab. */
  [[nodiscard]] const Dropdown& windowSizes() const;

  /** @brief Writes the descriptions and the rows enabled that follow the state
   * of the rows, on every tab. */
  void refreshDescriptions();

  /** @brief Lays every tab out again when the text size changed since they
   * were built, the same tab open; the first row, TEXT SIZE on ACCESS, is
   * selected again. */
  void followTextSize();

 private:
  OptionsTabsParts parts_;
  SettingsService* service_;
  ISizableWindow* window_;
  TextSize builtSize_;
  /** @brief Set while tabs_ is built, so declared, and initialized, before
   * it. */
  OptionsRowHandles handles_;
  std::vector<Menu> tabs_;
  std::size_t openTab_ = 0;
};

}  // namespace rtype::client
