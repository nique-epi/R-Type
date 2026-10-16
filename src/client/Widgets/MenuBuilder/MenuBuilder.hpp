#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include "Button.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "TextField.hpp"

namespace rtype::client {

/**
 * @brief Assembles the content of a menu step by step, then places it with the
 * layout its frame shares with every other menu: screens say what a menu
 * holds, never where.
 *
 * A page is a column against the left side of the safe margin: the version, the
 * logo, the heading, the subtitle, a rule, the buttons and the fields, then the
 * line describing the selected bar and the status line under it; its
 * illustration sits against the right side of the margin, its footer in the
 * bottom-right corner. A dialog is a panel centered on the picture: its
 * heading, its message, then its buttons in a centered row. A form is a panel
 * centered on the picture too: its heading, its fields one under the other,
 * its buttons in a centered row, then the line describing the selected bar
 * and the status line under it; the pointer selects one of its bars only by
 * clicking it. Whatever the order of the calls, the parts are
 * laid in this order; a part the frame does not show is left out.
 *
 * The cancel button is the one MENU_CANCEL_KEY activates; backButton() adds
 * the one button that closes the screen on top, the same in every menu.
 */
class MenuBuilder {
 public:
  /**
   * @brief How a menu is laid out: a page is a column against the left side of
   * the safe margin; a dialog is a panel centered over the dimmed screens
   * below; a form is a panel centered over the sky, that the player fills in.
   */
  enum class Frame : std::uint8_t {
    Page,
    Dialog,
    Form,
  };

  explicit MenuBuilder(Frame frame);

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
  MenuBuilder& button(Button::Text text, std::function<void()> action);

  /**
   * @brief Adds a text field after the previous bars, FIELD_SIZE large: in the
   * column of a form, or of a page. MENU_CONFIRM_KEY runs its action, as on a
   * button. A dialog, which asks a question, must hold buttons only.
   */
  MenuBuilder& textField(Button::Text text, TextField field,
                         std::function<void()> action);

  /** @brief Adds a button that MENU_CANCEL_KEY activates too; the last one
   * asked for is the cancel button. */
  MenuBuilder& cancelButton(Button::Text text, std::function<void()> action);

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
  /** @brief A bar asked for, not placed yet: a button, or a field when it holds
   * one. */
  struct PendingButton {
    Button::Text text;
    std::function<void()> action;
    std::optional<TextField> field;
  };

  void placeColumn(Menu::Content& content);
  void placeDialog(Menu::Content& content);
  void placeForm(Menu::Content& content);

  Frame frame_;
  std::optional<std::string> version_;
  std::optional<Menu::Picture> logo_;
  std::optional<std::string> heading_;
  std::optional<std::string> subtitle_;
  std::optional<std::string> message_;
  std::vector<PendingButton> buttons_;
  std::optional<std::size_t> cancelButton_;
  std::optional<sf::Vector2i> illustration_;
  std::optional<std::string> footer_;
};

}  // namespace rtype::client
