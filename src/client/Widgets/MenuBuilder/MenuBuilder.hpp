#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Button.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "RowControl.hpp"
#include "TabStrip.hpp"
#include "TextField.hpp"

namespace rtype::client {

class ISoundPlayer;

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
 * A tabbed panel is the heading and its rule as on a page, its tabs under the
 * rule, the panel of its rows under the tabs, then its bars in a row under the
 * panel and the line describing the selected bar or row on their right. Its
 * illustration sits in the panel under the rows, its notice in the top-right
 * corner, its status line at the end of the rule. Left and right step the
 * value of the selected row.
 *
 * A room is the heading and a rule across the safe margin, an illustration as
 * wide under the rule, its bars centered in a row under the illustration with
 * the status line centered under them, and its cancel button as a chip in the
 * bottom-right corner of the safe margin, showing the keycap of
 * MENU_CANCEL_KEY. A room writes no description: one given is left out.
 *
 * The cancel button is the one MENU_CANCEL_KEY activates; backButton() adds
 * the one button that closes the screen on top, the same in every menu.
 */
class MenuBuilder {
 public:
  /**
   * @brief How a menu is laid out: a page is a column against the left side of
   * the safe margin; a dialog is a panel centered over the dimmed screens
   * below; a form is a panel centered over the sky, that the player fills in;
   * a tabbed panel is a panel of rows under tabs, each row setting a value; a
   * room is a picture across the screen with its bars centered under it.
   */
  enum class Frame : std::uint8_t {
    Page,
    Dialog,
    Form,
    TabbedPanel,
    Room,
  };

  /** @param sounds Plays the click of the menu built, and must outlive it;
   * a menu built without one is silent. */
  explicit MenuBuilder(Frame frame, ISoundPlayer* sounds = nullptr);

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

  /** @brief Adds a row to a tabbed panel, after the previous ones: its label
   * on the left, its control on the right. Only a tabbed panel holds rows. */
  MenuBuilder& row(Button::Text text, std::unique_ptr<RowControl> control);

  /**
   * @brief Puts tabs over the rows of a tabbed panel.
   *
   * @param openTab Order of the tab this menu shows.
   * @param keys Names written on the keycaps of PREVIOUS_TAB_KEY and
   * NEXT_TAB_KEY.
   * @param openTabAt Run with the order of a tab a click or one of those keys
   * asks for.
   */
  MenuBuilder& tabs(std::vector<std::string> labels, std::size_t openTab,
                    TabStrip::KeyNames keys,
                    std::function<void(std::size_t)> openTabAt);

  /** @brief Dims the screens below first, as when the menu opens over a game
   * that goes on. */
  MenuBuilder& dimScreensBelow();

  /** @brief Small line in the top-right corner of a tabbed panel. */
  MenuBuilder& notice(std::string text);

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
    std::unique_ptr<RowControl> control;
  };

  /** @brief Where the next row and the next bar of a tabbed panel go: rows
   * down its panel, bars rightwards under it. */
  struct PanelCursor {
    int rowTop{};
    int barLeft{};
  };

  void placeColumn(Menu::Content& content);
  void placeDialog(Menu::Content& content);
  void placeForm(Menu::Content& content);
  void placeTabbedPanel(Menu::Content& content);
  void placeRoom(Menu::Content& content);

  /** @returns Where the chip of a room lies: as wide as its label and its
   * keycap need, against the bottom-right corner of the safe margin. */
  [[nodiscard]] static sf::IntRect chipArea(const Button::Text& text);

  /** @brief Places the heading against the top-left corner of the safe
   * margin and, under it, a rule of @p ruleSize.
   * @returns The top of the rule. */
  int placeHeadingAndRule(Menu::Content& content, sf::Vector2i ruleSize);

  /** @brief The heading of a tabbed panel and its rule, the status line at
   * the end of the rule, and the notice in the top-right corner. */
  void placePanelHeader(Menu::Content& content);

  /** @brief The panel of rows, the tabs over it, and the keys that move from
   * row to row. */
  void placeRowsPanel(Menu::Content& content);

  /**
   * @brief Places the rows down the panel and the bars in a row under it, in
   * the order they were asked for.
   * @returns Where the next row and the next bar would go.
   */
  [[nodiscard]] PanelCursor placeRowsAndBars(Menu::Content& content);

  /** @brief The description right of the last bar, and the illustration under
   * the last row. */
  void placeDescriptionAndIllustration(Menu::Content& content, PanelCursor end);

  Frame frame_;
  ISoundPlayer* sounds_;
  std::optional<std::string> version_;
  std::optional<Menu::Picture> logo_;
  std::optional<std::string> heading_;
  std::optional<std::string> subtitle_;
  std::optional<std::string> message_;
  std::vector<PendingButton> buttons_;
  std::optional<std::size_t> cancelButton_;
  std::optional<sf::Vector2i> illustration_;
  std::optional<std::string> footer_;
  std::optional<TabStrip> tabStrip_;
  std::function<void(std::size_t)> tabOpener_;
  bool dimmed_ = false;
  std::optional<std::string> notice_;
};

}  // namespace rtype::client
