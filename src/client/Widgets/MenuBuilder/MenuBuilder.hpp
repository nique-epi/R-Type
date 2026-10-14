#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include "ButtonText.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuContent.hpp"
#include "MenuFrame.hpp"

namespace rtype::client {

/**
 * @brief Assembles the content of a menu step by step, then places it with the
 * layout its frame shares with every other menu: screens say what a menu
 * holds, never where.
 *
 * A page is a column against the left side of the safe margin: the version, the
 * logo, the heading, the subtitle, a rule, the buttons, then the line
 * describing the selected button; its illustration sits against the right side
 * of the margin, its footer in the bottom-right corner. A dialog is a panel
 * centered on the picture: its heading, its message, then its buttons in a
 * centered row. Whatever the order of the calls, the parts are laid in this
 * order; a part the frame does not show is left out.
 *
 * The cancel button is the one MENU_CANCEL_KEY activates; backButton() adds
 * the one button that closes the screen on top, the same in every menu.
 */
class MenuBuilder {
 public:
  explicit MenuBuilder(MenuFrame frame);

  /** @brief Small line at the top of a page, such as the version of the
   * game. */
  MenuBuilder& version(std::string text);

  /** @brief Image heading a page, shown whole. @param size Its size, in
   * pixels. */
  MenuBuilder& logo(std::string assetId, sf::Vector2i size);

  /** @brief Line heading a page, or title of a dialog. */
  MenuBuilder& heading(std::string text);

  /** @brief Line under the heading of a page. */
  MenuBuilder& subtitle(std::string text);

  /** @brief Question of a dialog. */
  MenuBuilder& message(std::string text);

  /** @brief Adds a button after the previous ones. */
  MenuBuilder& button(ButtonText text, std::function<void()> action);

  /** @brief Adds a button that MENU_CANCEL_KEY activates too; the last one
   * asked for is the cancel button. */
  MenuBuilder& cancelButton(ButtonText text, std::function<void()> action);

  /**
   * @brief Adds BACK, cancel button that closes the screen on top.
   *
   * @param navigator Screens around the menu, which must outlive it.
   */
  MenuBuilder& backButton(IScreenNavigator& navigator);

  /** @brief Reserves an area of this size, in pixels, against the right side
   * of a page, for a picture its screen paints. */
  MenuBuilder& illustration(sf::Vector2i size);

  /** @brief Small line in the bottom-right corner of a page. */
  MenuBuilder& footer(std::string text);

  /** @brief Places every part and hands them to a menu; call it once, since
   * the actions move into the menu. */
  [[nodiscard]] Menu build();

 private:
  /** @brief A button asked for, not placed yet. */
  struct PendingButton {
    ButtonText text;
    std::function<void()> action;
  };

  void placeColumn(MenuContent& content);
  void placeDialog(MenuContent& content);

  MenuFrame frame_;
  std::optional<std::string> version_;
  std::optional<MenuPicture> logo_;
  std::optional<std::string> heading_;
  std::optional<std::string> subtitle_;
  std::optional<std::string> message_;
  std::vector<PendingButton> buttons_;
  std::optional<std::size_t> cancelButton_;
  std::optional<sf::Vector2i> illustration_;
  std::optional<std::string> footer_;
};

}  // namespace rtype::client
