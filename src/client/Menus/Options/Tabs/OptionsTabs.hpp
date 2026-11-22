#pragma once

#include <cstddef>
#include <optional>
#include <vector>
#include "ISizableWindow.hpp"
#include "Language.hpp"
#include "Menu.hpp"
#include "OptionsRowHandles.hpp"
#include "OptionsTabsParts.hpp"
#include "SelectedRow.hpp"
#include "SettingsService.hpp"

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

  /** @brief Builds every tab again in the language now picked, when it is
   * not the one they were built in; the open tab and its selected row stay.
   * Called between two events, never while a menu handles one. */
  void followLanguage();

 private:
  /** @returns The row selected in the open tab, or nothing. */
  [[nodiscard]] std::optional<SelectedRow> selectedRow() const;

  OptionsTabsParts parts_;
  SettingsService* service_;
  ISizableWindow* window_;
  Language builtIn_;
  /** @brief Set while tabs_ is built, so declared, and initialized, before
   * it. */
  OptionsRowHandles handles_;
  std::vector<Menu> tabs_;
  std::size_t openTab_ = 0;
};

}  // namespace rtype::client
