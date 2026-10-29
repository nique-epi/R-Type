#pragma once

#include <cstddef>
#include <vector>
#include "ISizableWindow.hpp"
#include "Menu.hpp"
#include "OptionsRowHandles.hpp"
#include "OptionsTabsParts.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

class Dropdown;

/**
 * @brief The four tabs of the options, SOUND, CONTROLS, DISPLAY and ACCESS,
 * each a panel of rows over BACK, and the one that is open.
 *
 * The rows read and change the settings through the SettingsService. The
 * descriptions follow the state of the rows: the keys of an open list, a key
 * awaited, a key two actions share, the sound muted, a size too large for the
 * screen, what borderless does.
 */
class OptionsTabs {
 public:
  explicit OptionsTabs(OptionsTabsParts parts);

  /** @brief Opens @p tab; ignores a tab there is not. */
  void openTab(std::size_t tab);

  [[nodiscard]] Menu& openMenu();
  [[nodiscard]] const Menu& openMenu() const;

  /** @returns The list of window sizes of the DISPLAY tab. */
  [[nodiscard]] const Dropdown& windowSizes() const;

  /** @brief Writes the descriptions and the rows enabled that follow the state
   * of the rows, on every tab. */
  void refreshDescriptions();

 private:
  SettingsService* service_;
  ISizableWindow* window_;
  /** @brief Set while tabs_ is built, so declared, and initialized, before
   * it. */
  OptionsRowHandles handles_;
  std::vector<Menu> tabs_;
  std::size_t openTab_ = 0;
};

}  // namespace rtype::client
