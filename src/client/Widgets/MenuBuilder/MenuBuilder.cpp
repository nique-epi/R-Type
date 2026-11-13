#include "MenuBuilder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BarRowWidth.hpp"
#include "Button.hpp"
#include "IScreenNavigator.hpp"
#include "InterfaceConstants.hpp"
#include "KeyCap.hpp"
#include "Menu.hpp"
#include "PanelCursor.hpp"
#include "PendingButton.hpp"
#include "RowControl.hpp"
#include "RowScroll.hpp"
#include "RowWindow.hpp"
#include "TabStrip.hpp"
#include "TextField.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief A line of the column of a page: where it starts, how it is written,
 * and the space it leaves under it. */
struct ColumnLine {
  int top{};
  TextStyle style;
  int spaceBelow{};
};

/** @brief Where the parts of a form lie, from the top of its panel, and how
 * many of its bars are buttons. */
struct FormLayout {
  sf::Vector2i panelTopLeft;
  sf::Vector2i panelSize;
  int fieldsTop{};
  int buttonsTop{};
  int descriptionTop{};
  int statusTop{};
  int buttonCount{};
};

/** @returns The top of what comes under a line of a column, which is left out
 * when it was not asked for. */
int placeColumnLine(Menu::Content& content, std::optional<std::string>& text,
                    const ColumnLine& line) {
  if (!text.has_value()) {
    return line.top;
  }
  content.labels.emplace_back(std::move(*text),
                              sf::Vector2i(SAFE_MARGIN, line.top), line.style);
  return line.top + TEXT_LINE_HEIGHT + line.spaceBelow;
}

/** @returns The top of what comes under the bars of a column. */
int placeColumnBars(Menu::Content& content, std::vector<PendingButton>& bars,
                    int top) {
  for (PendingButton& pending : bars) {
    const sf::Vector2i size =
        pending.field.has_value() ? FIELD_SIZE : BUTTON_SIZE;
    content.buttons.emplace_back(
        std::move(pending.text), sf::IntRect({SAFE_MARGIN, top}, size),
        std::move(pending.action), std::move(pending.field));
    top += size.y + BUTTON_GAP;
  }
  return top;
}

/** @returns Where the parts of a form holding these bars lie. */
FormLayout formLayoutOf(const std::vector<PendingButton>& bars) {
  const int fieldCount = static_cast<int>(
      std::ranges::count_if(bars, [](const PendingButton& pending) noexcept {
        return pending.field.has_value();
      }));
  const int fieldsTop =
      FORM_HEADING_OFFSET.y + TEXT_LINE_HEIGHT + SPACE_BELOW_FORM_HEADING;
  const int buttonsTop = fieldsTop + (fieldCount * FIELD_SIZE.y) +
                         ((fieldCount - 1) * SPACE_BETWEEN_FIELDS) +
                         SPACE_AROUND_FORM_BUTTONS;
  const int descriptionTop =
      buttonsTop + FORM_BUTTON_SIZE.y + SPACE_AROUND_FORM_BUTTONS;
  const int statusTop =
      descriptionTop + TEXT_LINE_HEIGHT + SPACE_BELOW_DESCRIPTION;
  const sf::Vector2i panelSize(
      FORM_WIDTH, statusTop + TEXT_LINE_HEIGHT + FORM_HEADING_OFFSET.y);
  return FormLayout{
      .panelTopLeft = (INTERFACE_AREA.size - panelSize) / SIDES_PER_AXIS,
      .panelSize = panelSize,
      .fieldsTop = fieldsTop,
      .buttonsTop = buttonsTop,
      .descriptionTop = descriptionTop,
      .statusTop = statusTop,
      .buttonCount = static_cast<int>(bars.size()) - fieldCount};
}

/** @brief Places the fields of a form one under the other, its buttons in a
 * centered row under them. */
void placeFormBars(Menu::Content& content, std::vector<PendingButton>& bars,
                   const FormLayout& layout) {
  const sf::Vector2i panel = layout.panelTopLeft;
  const int rowWidth =
      barRowWidth(layout.buttonCount, FORM_BUTTON_SIZE, FORM_BUTTON_GAP);
  int fieldTop = panel.y + layout.fieldsTop;
  int buttonLeft = panel.x + ((FORM_WIDTH - rowWidth) / SIDES_PER_AXIS);
  for (PendingButton& pending : bars) {
    sf::IntRect area;
    if (pending.field.has_value()) {
      area =
          sf::IntRect({panel.x + FORM_HEADING_OFFSET.x, fieldTop}, FIELD_SIZE);
      fieldTop += FIELD_SIZE.y + SPACE_BETWEEN_FIELDS;
    } else {
      area = sf::IntRect({buttonLeft, panel.y + layout.buttonsTop},
                         FORM_BUTTON_SIZE);
      buttonLeft += FORM_BUTTON_SIZE.x + FORM_BUTTON_GAP;
    }
    content.buttons.emplace_back(std::move(pending.text), area,
                                 std::move(pending.action),
                                 std::move(pending.field));
  }
}

/** @brief Where the rows and the bars of a menu lie: the rows down from the
 * first, the bars rightwards from the first. */
struct RowsAndBars {
  sf::Vector2i firstRow;
  sf::Vector2i rowSize;
  sf::Vector2i firstBar;
  sf::Vector2i barSize;
  int barGap{};
};

/** @returns Where the next row and the next bar would go. */
PanelCursor placeRowsAndBarsOf(Menu::Content& content,
                               std::vector<PendingButton>& bars,
                               const RowsAndBars& place) {
  PanelCursor cursor{.rowTop = place.firstRow.y, .barLeft = place.firstBar.x};
  for (PendingButton& pending : bars) {
    if (pending.control != nullptr) {
      content.buttons.emplace_back(
          std::move(pending.text),
          sf::IntRect({place.firstRow.x, cursor.rowTop}, place.rowSize),
          std::move(pending.control));
      cursor.rowTop += place.rowSize.y + ROW_GAP;
    } else {
      content.buttons.emplace_back(
          std::move(pending.text),
          sf::IntRect({cursor.barLeft, place.firstBar.y}, place.barSize),
          std::move(pending.action), std::move(pending.field));
      cursor.barLeft += place.barSize.x + place.barGap;
    }
  }
  return cursor;
}

/** @returns The rows of @p content, when the panel of a tabbed panel holds
 * more of them than it shows: the first rows show, the rest scroll. */
std::optional<RowScroll> rowScrollOf(const Menu::Content& content) {
  const auto rows = static_cast<std::size_t>(std::ranges::count_if(
      content.buttons,
      [](const Button& button) { return button.control() != nullptr; }));
  if (rows <= PANEL_ROW_CAPACITY) {
    return std::nullopt;
  }
  const auto first = std::ranges::find_if(
      content.buttons,
      [](const Button& button) { return button.control() != nullptr; });
  return RowScroll{
      .rows = RowWindow{.first = 0, .shown = PANEL_ROW_CAPACITY, .count = rows},
      .firstButton = static_cast<std::size_t>(first - content.buttons.begin()),
      .pitch = ROW_PITCH,
      .viewport = PANEL_ROWS_VIEWPORT};
}

/** @brief Takes out of @p bars every bar asked for after the cancel button: a
 * tabbed panel lays them against the right end of its panel.
 * @returns Those bars, in the order they were asked for. */
std::vector<PendingButton> barsAfterCancel(
    std::vector<PendingButton>& bars, std::optional<std::size_t> cancelButton) {
  std::vector<PendingButton> trailing;
  if (!cancelButton.has_value() || *cancelButton + 1 >= bars.size()) {
    return trailing;
  }
  const auto first =
      bars.begin() + static_cast<std::ptrdiff_t>(*cancelButton + 1);
  trailing.assign(std::make_move_iterator(first),
                  std::make_move_iterator(bars.end()));
  bars.erase(first, bars.end());
  return trailing;
}

/** @brief Places @p bars in a row under the panel of rows, the last one ending
 * at the right end of the panel. */
void placeBarsAtPanelEnd(Menu::Content& content,
                         std::vector<PendingButton>& bars) {
  const int rowWidth = barRowWidth(static_cast<int>(bars.size()),
                                   PANEL_BAR_SIZE, SPACE_BESIDE_PANEL_BARS);
  int barLeft = ROWS_PANEL_TOP_LEFT.x + ROWS_PANEL_SIZE.x - rowWidth;
  for (PendingButton& pending : bars) {
    content.buttons.emplace_back(
        std::move(pending.text),
        sf::IntRect({barLeft, PANEL_BARS_TOP}, PANEL_BAR_SIZE),
        std::move(pending.action), std::move(pending.field));
    barLeft += PANEL_BAR_SIZE.x + SPACE_BESIDE_PANEL_BARS;
  }
}

/** @returns How many of @p bars are rows. */
int rowCountOf(const std::vector<PendingButton>& bars) {
  return static_cast<int>(
      std::ranges::count_if(bars, [](const PendingButton& pending) noexcept {
        return pending.control != nullptr;
      }));
}

/** @returns How far down its panel a dialog of @p rowCount rows places its
 * buttons: DIALOG_BUTTONS_TOP under a message, ROW_GAP under the last row
 * otherwise. */
int dialogButtonsTopOf(int rowCount) {
  if (rowCount == 0) {
    return DIALOG_BUTTONS_TOP;
  }
  return DIALOG_ROWS_TOP + (rowCount * DIALOG_ROW_PITCH) + ROW_GAP;
}

/** @returns The size of a dialog of @p rowCount rows: DIALOG_SIZE under a
 * message, and with rows as tall as ends DIALOG_MARGIN_UNDER_BUTTONS under its
 * buttons. */
sf::Vector2i dialogSizeOf(int rowCount) {
  if (rowCount == 0) {
    return DIALOG_SIZE;
  }
  return {DIALOG_SIZE.x, dialogButtonsTopOf(rowCount) + DIALOG_BUTTON_SIZE.y +
                             DIALOG_MARGIN_UNDER_BUTTONS};
}

/** @brief Places the rows of a dialog under its heading and its buttons in a
 * centered row; rows keep Left and Right to step their values. */
void placeDialogBars(Menu::Content& content, std::vector<PendingButton>& bars,
                     sf::Vector2i panel) {
  const int rowCount = rowCountOf(bars);
  const int barCount = static_cast<int>(bars.size()) - rowCount;
  const int rowWidth =
      barRowWidth(barCount, DIALOG_BUTTON_SIZE, DIALOG_BUTTON_GAP);
  placeRowsAndBarsOf(
      content, bars,
      RowsAndBars{
          .firstRow = {panel.x + DIALOG_ROW_INSET, panel.y + DIALOG_ROWS_TOP},
          .rowSize = DIALOG_ROW_SIZE,
          .firstBar = {panel.x + ((DIALOG_SIZE.x - rowWidth) / SIDES_PER_AXIS),
                       panel.y + dialogButtonsTopOf(rowCount)},
          .barSize = DIALOG_BUTTON_SIZE,
          .barGap = DIALOG_BUTTON_GAP});
  if (rowCount > 0) {
    content.nextKeys = PANEL_NEXT_KEYS;
    content.previousKeys = PANEL_PREVIOUS_KEYS;
  }
}

}  // namespace

MenuBuilder::MenuBuilder(Frame frame, ISoundPlayer* sounds)
    : frame_(frame), sounds_(sounds) {}

MenuBuilder& MenuBuilder::version(std::string text) {
  version_ = std::move(text);
  return *this;
}

MenuBuilder& MenuBuilder::logo(std::string assetId, sf::Vector2i size) {
  logo_ = Menu::Picture{.assetId = std::move(assetId), .area = {{0, 0}, size}};
  return *this;
}

MenuBuilder& MenuBuilder::heading(std::string text) {
  heading_ = std::move(text);
  return *this;
}

MenuBuilder& MenuBuilder::subtitle(std::string text) {
  subtitle_ = std::move(text);
  return *this;
}

MenuBuilder& MenuBuilder::message(std::string text) {
  message_ = std::move(text);
  return *this;
}

MenuBuilder& MenuBuilder::button(Button::Text text,
                                 std::function<void()> action) {
  buttons_.push_back({.text = std::move(text),
                      .action = std::move(action),
                      .field = std::nullopt,
                      .control = nullptr});
  return *this;
}

MenuBuilder& MenuBuilder::textField(Button::Text text, TextField field,
                                    std::function<void()> action) {
  buttons_.push_back({.text = std::move(text),
                      .action = std::move(action),
                      .field = std::move(field),
                      .control = nullptr});
  return *this;
}

MenuBuilder& MenuBuilder::cancelButton(Button::Text text,
                                       std::function<void()> action) {
  cancelButton_ = buttons_.size();
  return button(std::move(text), std::move(action));
}

MenuBuilder& MenuBuilder::backButton(IScreenNavigator& navigator) {
  return cancelButton({.label = BACK_LABEL}, [&navigator] { navigator.pop(); });
}

MenuBuilder& MenuBuilder::illustration(sf::Vector2i size) {
  illustration_ = size;
  return *this;
}

MenuBuilder& MenuBuilder::footer(std::string text) {
  footer_ = std::move(text);
  return *this;
}

MenuBuilder& MenuBuilder::row(Button::Text text,
                              std::unique_ptr<RowControl> control) {
  buttons_.push_back({.text = std::move(text),
                      .action = nullptr,
                      .field = std::nullopt,
                      .control = std::move(control)});
  return *this;
}

MenuBuilder& MenuBuilder::tabs(std::vector<std::string> labels,
                               std::size_t openTab, TabStrip::KeyNames keys,
                               std::function<void(std::size_t)> openTabAt) {
  tabStrip_.emplace(std::move(labels), openTab, std::move(keys),
                    TAB_STRIP_TOP_LEFT);
  tabOpener_ = std::move(openTabAt);
  return *this;
}

MenuBuilder& MenuBuilder::dimScreensBelow() {
  dimmed_ = true;
  return *this;
}

MenuBuilder& MenuBuilder::notice(std::string text) {
  notice_ = std::move(text);
  return *this;
}

Menu MenuBuilder::build() {
  Menu::Content content;
  content.cancelButton = cancelButton_;
  content.dimmed = dimmed_;
  switch (frame_) {
    case Frame::Dialog:
      content.dimmed = true;
      placeDialog(content);
      break;
    case Frame::Page:
      placeColumn(content);
      break;
    case Frame::Form:
      placeForm(content);
      break;
    case Frame::TabbedPanel:
      placeTabbedPanel(content);
      break;
    case Frame::Room:
      placeRoom(content);
      break;
  }
  return Menu(std::move(content), sounds_);
}

void MenuBuilder::placeColumn(Menu::Content& content) {
  int top = placeColumnLine(content, version_,
                            {.top = SAFE_MARGIN,
                             .style = VERSION_STYLE,
                             .spaceBelow = SPACE_BELOW_VERSION});
  if (logo_.has_value()) {
    logo_->area.position = {SAFE_MARGIN, top};
    top += logo_->area.size.y + SPACE_BELOW_HEADING;
    content.picture = std::move(logo_);
  }
  top = placeColumnLine(content, heading_,
                        {.top = top,
                         .style = PAGE_LINE_STYLE,
                         .spaceBelow = SPACE_BELOW_HEADING});
  top = placeColumnLine(content, subtitle_,
                        {.top = top,
                         .style = PAGE_LINE_STYLE,
                         .spaceBelow = SPACE_BELOW_SUBTITLE});
  content.rule = sf::IntRect({SAFE_MARGIN, top}, RULE_SIZE);
  top =
      placeColumnBars(content, buttons_, top + RULE_SIZE.y + SPACE_BELOW_RULE);
  content.descriptionAnchor = sf::Vector2i(SAFE_MARGIN, top);
  const int right = INTERFACE_AREA.size.x - SAFE_MARGIN;
  if (illustration_.has_value()) {
    content.illustration = sf::IntRect(
        {right - illustration_->x, ILLUSTRATION_TOP}, *illustration_);
  }
  if (footer_.has_value()) {
    content.labels.emplace_back(std::move(*footer_),
                                sf::Vector2i(right, FOOTER_TOP), FOOTER_STYLE);
  }
}

void MenuBuilder::placeDialog(Menu::Content& content) {
  const sf::Vector2i size = dialogSizeOf(rowCountOf(buttons_));
  const sf::Vector2i panelTopLeft =
      (INTERFACE_AREA.size - size) / SIDES_PER_AXIS;
  content.panel = sf::IntRect(panelTopLeft, size);
  if (heading_.has_value()) {
    content.labels.emplace_back(std::move(*heading_),
                                panelTopLeft + DIALOG_HEADING_OFFSET,
                                DIALOG_HEADING_STYLE);
  }
  if (message_.has_value()) {
    content.labels.emplace_back(std::move(*message_),
                                panelTopLeft + DIALOG_MESSAGE_OFFSET,
                                DIALOG_MESSAGE_STYLE);
  }
  placeDialogBars(content, buttons_, panelTopLeft);
}

void MenuBuilder::placeForm(Menu::Content& content) {
  const FormLayout layout = formLayoutOf(buttons_);
  const sf::Vector2i panel = layout.panelTopLeft;
  content.panel = sf::IntRect(panel, layout.panelSize);
  if (heading_.has_value()) {
    content.labels.emplace_back(std::move(*heading_),
                                panel + FORM_HEADING_OFFSET,
                                DIALOG_HEADING_STYLE);
  }
  placeFormBars(content, buttons_, layout);
  const int left = panel.x + FORM_HEADING_OFFSET.x;
  content.descriptionAnchor =
      sf::Vector2i(left, panel.y + layout.descriptionTop);
  content.descriptionStyle = FORM_DESCRIPTION_STYLE;
  content.selectsOnHover = false;
  content.statusAnchor = sf::Vector2i(left, panel.y + layout.statusTop);
}

void MenuBuilder::placeTabbedPanel(Menu::Content& content) {
  placePanelHeader(content);
  placeRowsPanel(content);
  const PanelCursor end = placeRowsAndBars(content);
  placeDescriptionAndIllustration(content, end);
}

int MenuBuilder::placeHeadingAndRule(Menu::Content& content,
                                     sf::Vector2i ruleSize) {
  if (heading_.has_value()) {
    content.labels.emplace_back(std::move(*heading_),
                                sf::Vector2i(SAFE_MARGIN, SAFE_MARGIN),
                                PAGE_LINE_STYLE);
  }
  const int ruleTop = SAFE_MARGIN + TEXT_LINE_HEIGHT + SPACE_BELOW_HEADING;
  content.rule = sf::IntRect({SAFE_MARGIN, ruleTop}, ruleSize);
  return ruleTop;
}

void MenuBuilder::placePanelHeader(Menu::Content& content) {
  const int ruleTop = placeHeadingAndRule(content, RULE_SIZE);
  content.statusAnchor =
      sf::Vector2i(SAFE_MARGIN + RULE_SIZE.x, ruleTop) + RULE_STAMP_OFFSET;
  content.statusStyle = STAMP_STYLE;
  if (notice_.has_value()) {
    content.labels.emplace_back(
        std::move(*notice_),
        sf::Vector2i(INTERFACE_AREA.size.x - SAFE_MARGIN, SAFE_MARGIN),
        NOTICE_STYLE);
  }
}

void MenuBuilder::placeRowsPanel(Menu::Content& content) {
  content.panel = sf::IntRect(ROWS_PANEL_TOP_LEFT, ROWS_PANEL_SIZE);
  content.tabs = std::move(tabStrip_);
  content.openTab = std::move(tabOpener_);
  content.nextKeys = PANEL_NEXT_KEYS;
  content.previousKeys = PANEL_PREVIOUS_KEYS;
}

PanelCursor MenuBuilder::placeRowsAndBars(Menu::Content& content) {
  std::vector<PendingButton> trailing =
      barsAfterCancel(buttons_, cancelButton_);
  const PanelCursor end =
      placeRowsAndBarsOf(content, buttons_,
                         RowsAndBars{.firstRow = FIRST_ROW_TOP_LEFT,
                                     .rowSize = ROW_SIZE,
                                     .firstBar = {SAFE_MARGIN, PANEL_BARS_TOP},
                                     .barSize = PANEL_BAR_SIZE,
                                     .barGap = SPACE_BESIDE_PANEL_BARS});
  placeBarsAtPanelEnd(content, trailing);
  content.rowScroll = rowScrollOf(content);
  return end;
}

void MenuBuilder::placeDescriptionAndIllustration(Menu::Content& content,
                                                  PanelCursor end) {
  content.descriptionAnchor =
      sf::Vector2i(end.barLeft, PANEL_BARS_TOP + BUTTON_TEXT_OFFSET.y);
  if (illustration_.has_value()) {
    content.illustration =
        sf::IntRect({FIRST_ROW_TOP_LEFT.x + ROW_PADDING,
                     end.rowTop + SPACE_ABOVE_PANEL_ILLUSTRATION},
                    *illustration_);
  }
}

void MenuBuilder::placeRoom(Menu::Content& content) {
  placeHeadingAndRule(content, ROOM_RULE_SIZE);
  int barsTop = ROOM_ILLUSTRATION_TOP;
  if (illustration_.has_value()) {
    content.illustration =
        sf::IntRect({SAFE_MARGIN, ROOM_ILLUSTRATION_TOP}, *illustration_);
    barsTop += illustration_->y + SPACE_ABOVE_ROOM_BARS;
  }
  const int barCount =
      static_cast<int>(buttons_.size()) - (cancelButton_.has_value() ? 1 : 0);
  const int rowWidth = barRowWidth(barCount, BUTTON_SIZE, BUTTON_GAP);
  int barLeft = (INTERFACE_AREA.size.x - rowWidth) / SIDES_PER_AXIS;
  for (std::size_t index = 0; index < buttons_.size(); ++index) {
    PendingButton& pending = buttons_[index];
    sf::IntRect area;
    if (cancelButton_ == index) {
      pending.text.keyName = CANCEL_KEY_NAME;
      area = chipArea(pending.text);
    } else {
      area = sf::IntRect({barLeft, barsTop}, BUTTON_SIZE);
      barLeft += BUTTON_SIZE.x + BUTTON_GAP;
    }
    content.buttons.emplace_back(std::move(pending.text), area,
                                 std::move(pending.action),
                                 std::move(pending.field));
  }
  content.statusAnchor = sf::Vector2i(INTERFACE_AREA.size.x / SIDES_PER_AXIS,
                                      barsTop + BUTTON_SIZE.y + BUTTON_GAP);
  content.statusStyle = ROOM_STATUS_STYLE;
}

sf::IntRect MenuBuilder::chipArea(const Button::Text& text,
                                  std::optional<int> right) {
  const int width =
      BUTTON_TEXT_OFFSET.x + regularTextWidth(text.label) + CHIP_KEY_CAP_GAP +
      KeyCap(text.keyName, KeyCap::Edge::Plain).width() + BAR_KEY_CAP_INSET;
  const sf::Vector2i bottomRight =
      INTERFACE_AREA.size - sf::Vector2i(SAFE_MARGIN, SAFE_MARGIN);
  return {{right.value_or(bottomRight.x) - width, bottomRight.y - CHIP_HEIGHT},
          {width, CHIP_HEIGHT}};
}

}  // namespace rtype::client
