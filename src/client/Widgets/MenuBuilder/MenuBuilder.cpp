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
#include "DialogLayout.hpp"
#include "DialogLayouts.hpp"
#include "DialogText.hpp"
#include "IScreenNavigator.hpp"
#include "InterfaceConstants.hpp"
#include "KeyCap.hpp"
#include "Menu.hpp"
#include "PanelCursor.hpp"
#include "PanelLayouts.hpp"
#include "PendingButton.hpp"
#include "RowControl.hpp"
#include "RowScroll.hpp"
#include "RowWindow.hpp"
#include "TabStrip.hpp"
#include "TabbedPanelLayout.hpp"
#include "TextField.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
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
  int lineHeight = TEXT_LINE_HEIGHT;
};

/** @brief What a dialog was asked to write, as the builder holds it. */
struct DialogParts {
  std::reference_wrapper<const std::optional<std::string>> heading;
  std::reference_wrapper<const std::optional<std::string>> message;
  std::reference_wrapper<const std::vector<PendingButton>> bars;
};

/** @brief A dialog laid out and where its panel lies. */
struct PlacedDialog {
  DialogLayout layout;
  sf::Vector2i topLeft;
};

/** @brief Where the bars of a page laid in two columns start, and how tall
 * each is. */
struct BarGrid {
  int top{};
  int barHeight{};
};

/** @brief What closes a page: the area its screen paints in, its footer, and
 * the text size the page is laid out at. */
struct PageEnd {
  std::optional<sf::Vector2i> illustration;
  std::optional<std::string> footer;
  TextMetrics metrics;
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
  return line.top + line.lineHeight + line.spaceBelow;
}

/** @returns The top of what comes under the bars of a page laid in two
 * columns, as a page is above the size the fonts are drawn at, when one
 * column of taller bars no longer fits over the description. */
int placeBarsInTwoColumns(Menu::Content& content,
                          std::vector<PendingButton>& bars,
                          const BarGrid& grid) {
  const int width =
      (INTERFACE_AREA.size.x - (SIDES_PER_AXIS * SAFE_MARGIN) - BUTTON_GAP) /
      SIDES_PER_AXIS;
  int row = 0;
  int column = 0;
  for (PendingButton& pending : bars) {
    const sf::Vector2i topLeft(
        SAFE_MARGIN + (column * (width + BUTTON_GAP)),
        grid.top + (row * (grid.barHeight + BUTTON_GAP)));
    content.buttons.emplace_back(
        std::move(pending.text), sf::IntRect(topLeft, {width, grid.barHeight}),
        std::move(pending.action), std::move(pending.field));
    column = (column + 1) % SIDES_PER_AXIS;
    row += column == 0 ? 1 : 0;
  }
  const int rows = row + (column == 0 ? 0 : 1);
  return grid.top + (rows * (grid.barHeight + BUTTON_GAP));
}

/** @brief Places the area the screen of a page paints in, at its right, as
 * the page was drawn at the size of the fonts and never above it, where two
 * columns of bars take its room; then the footer, its last line on the row
 * it was drawn on. */
void placePageEnd(Menu::Content& content, PageEnd end) {
  const int right = INTERFACE_AREA.size.x - SAFE_MARGIN;
  if (end.illustration.has_value() && end.metrics == TextMetrics{}) {
    content.illustration = sf::IntRect(
        {right - end.illustration->x, ILLUSTRATION_TOP}, *end.illustration);
  }
  if (end.footer.has_value()) {
    content.labels.emplace_back(
        std::move(*end.footer),
        sf::Vector2i(right,
                     FOOTER_TOP + TEXT_LINE_HEIGHT - lineHeightOf(end.metrics)),
        FOOTER_STYLE);
  }
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
std::optional<RowScroll> rowScrollOf(const Menu::Content& content,
                                     const TabbedPanelLayout& layout) {
  const auto rows = static_cast<std::size_t>(std::ranges::count_if(
      content.buttons,
      [](const Button& button) { return button.control() != nullptr; }));
  if (rows <= layout.rowCapacity) {
    return std::nullopt;
  }
  const auto first = std::ranges::find_if(
      content.buttons,
      [](const Button& button) { return button.control() != nullptr; });
  return RowScroll{
      .rows = RowWindow{.first = 0, .shown = layout.rowCapacity, .count = rows},
      .firstButton = static_cast<std::size_t>(first - content.buttons.begin()),
      .pitch = layout.rowSize.y + ROW_GAP,
      .viewport = rowsViewportOf(layout)};
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
                         std::vector<PendingButton>& bars,
                         const TabbedPanelLayout& layout) {
  const int rowWidth = barRowWidth(static_cast<int>(bars.size()),
                                   layout.barSize, SPACE_BESIDE_PANEL_BARS);
  int barLeft = layout.panel.position.x + layout.panel.size.x - rowWidth;
  for (PendingButton& pending : bars) {
    content.buttons.emplace_back(
        std::move(pending.text),
        sf::IntRect({barLeft, layout.barsTop}, layout.barSize),
        std::move(pending.action), std::move(pending.field));
    barLeft += layout.barSize.x + SPACE_BESIDE_PANEL_BARS;
  }
}

/** @returns How wide every bar of a row of @p bars is: @p narrowest, or the
 * widest label of them and its padding when that is wider. */
int barWidthFor(const std::vector<PendingButton>& bars, int narrowest,
                TextMetrics metrics) {
  int width = narrowest;
  for (const PendingButton& pending : bars) {
    if (pending.control == nullptr) {
      width = std::max(width, regularTextWidth(metrics, pending.text.label) +
                                  (SIDES_PER_AXIS * BUTTON_TEXT_OFFSET.x));
    }
  }
  return width;
}

/** @returns Where the description of the selected row is written: beside the
 * bars, on the middle row of a bar as the options were drawn, or at the top
 * of the strip of the bars when it wraps on two lines at a larger size. */
sf::Vector2i descriptionAnchorOf(const TabbedPanelLayout& layout,
                                 PanelCursor end, TextMetrics metrics) {
  if (metrics != TextMetrics{}) {
    return {end.barLeft, layout.barsTop};
  }
  return {
      end.barLeft,
      centeredTextTop(
          sf::IntRect({end.barLeft, layout.barsTop}, layout.barSize), metrics)};
}

/** @returns How many of @p bars are rows. */
int rowCountOf(const std::vector<PendingButton>& bars) {
  return static_cast<int>(
      std::ranges::count_if(bars, [](const PendingButton& pending) noexcept {
        return pending.control != nullptr;
      }));
}

/** @returns What a dialog writes, read from the parts asked of the
 * builder. */
DialogText dialogTextOf(const DialogParts& parts) {
  const std::optional<std::string>& heading = parts.heading.get();
  const std::optional<std::string>& message = parts.message.get();
  DialogText text{.heading = heading.has_value() ? std::string_view(*heading)
                                                 : std::string_view(),
                  .message = message.has_value() ? std::string_view(*message)
                                                 : std::string_view(),
                  .rowCount = rowCountOf(parts.bars.get()),
                  .barLabels = {}};
  for (const PendingButton& pending : parts.bars.get()) {
    if (pending.control == nullptr) {
      text.barLabels.emplace_back(pending.text.label);
    }
  }
  return text;
}

/** @brief Writes the message of a dialog, one label per line of its layout,
 * from where the layout puts the message. */
void placeMessageLines(Menu::Content& content, const DialogLayout& layout,
                       sf::Vector2i panel) {
  int top = panel.y + layout.messageTop;
  for (const std::string& line : layout.messageLines) {
    content.labels.emplace_back(
        line, sf::Vector2i(panel.x + DIALOG_MESSAGE_OFFSET.x, top),
        DIALOG_MESSAGE_STYLE);
    top += layout.messagePitch;
  }
}

/** @brief Places the rows of a dialog under its heading and its buttons in a
 * centered row; rows keep Left and Right to step their values. */
void placeDialogBars(Menu::Content& content, std::vector<PendingButton>& bars,
                     const PlacedDialog& dialog) {
  const int rowCount = rowCountOf(bars);
  const int barCount = static_cast<int>(bars.size()) - rowCount;
  const DialogLayout& layout = dialog.layout;
  const sf::Vector2i panel = dialog.topLeft;
  const int rowWidth =
      barRowWidth(barCount, layout.buttonSize, DIALOG_BUTTON_GAP);
  placeRowsAndBarsOf(
      content, bars,
      RowsAndBars{
          .firstRow = {panel.x + DIALOG_ROW_INSET, panel.y + layout.rowsTop},
          .rowSize = layout.rowSize,
          .firstBar = {panel.x + ((layout.size.x - rowWidth) / SIDES_PER_AXIS),
                       panel.y + layout.buttonsTop},
          .barSize = layout.buttonSize,
          .barGap = DIALOG_BUTTON_GAP});
  if (rowCount > 0) {
    content.nextKeys = PANEL_NEXT_KEYS;
    content.previousKeys = PANEL_PREVIOUS_KEYS;
  }
}

}  // namespace

MenuBuilder::MenuBuilder(Frame frame, ISoundPlayer* sounds, TextMetrics metrics)
    : frame_(frame), sounds_(sounds), metrics_(metrics) {}

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
                    tabbedPanelLayoutOf(metrics_).tabsTopLeft);
  tabStrip_->setTextSize(metrics_);
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
  for (PendingButton& pending : buttons_) {
    pending.text.size = metrics_;
  }
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
  const int lineHeight = lineHeightOf(metrics_);
  int top = placeColumnLine(content, version_,
                            {.top = SAFE_MARGIN,
                             .style = VERSION_STYLE,
                             .spaceBelow = SPACE_BELOW_VERSION,
                             .lineHeight = lineHeight});
  if (logo_.has_value()) {
    logo_->area.position = {SAFE_MARGIN, top};
    top += logo_->area.size.y + SPACE_BELOW_HEADING;
    content.picture = std::move(logo_);
  }
  top = placeColumnLine(content, heading_,
                        {.top = top,
                         .style = PAGE_LINE_STYLE,
                         .spaceBelow = SPACE_BELOW_HEADING,
                         .lineHeight = lineHeight});
  top = placeColumnLine(content, subtitle_,
                        {.top = top,
                         .style = PAGE_LINE_STYLE,
                         .spaceBelow = SPACE_BELOW_SUBTITLE,
                         .lineHeight = lineHeight});
  content.rule = sf::IntRect({SAFE_MARGIN, top}, RULE_SIZE);
  top += RULE_SIZE.y + SPACE_BELOW_RULE;
  if (metrics_ == TextMetrics{}) {
    top = placeColumnBars(content, buttons_, top);
  } else {
    top = placeBarsInTwoColumns(
        content, buttons_,
        BarGrid{.top = top,
                .barHeight = textBoxHeight(BUTTON_SIZE.y, metrics_)});
    content.descriptionWidth =
        INTERFACE_AREA.size.x - (SIDES_PER_AXIS * SAFE_MARGIN);
  }
  content.descriptionAnchor = sf::Vector2i(SAFE_MARGIN, top);
  placePageEnd(content, PageEnd{.illustration = illustration_,
                                .footer = std::move(footer_),
                                .metrics = metrics_});
}

void MenuBuilder::placeDialog(Menu::Content& content) {
  const DialogLayout layout = dialogLayoutOf(
      dialogTextOf(DialogParts{
          .heading = heading_, .message = message_, .bars = buttons_}),
      metrics_);
  const sf::Vector2i panelTopLeft =
      (INTERFACE_AREA.size - layout.size) / SIDES_PER_AXIS;
  content.panel = sf::IntRect(panelTopLeft, layout.size);
  if (heading_.has_value()) {
    content.labels.emplace_back(std::move(*heading_),
                                panelTopLeft + DIALOG_HEADING_OFFSET,
                                DIALOG_HEADING_STYLE);
  }
  if (message_.has_value()) {
    placeMessageLines(content, layout, panelTopLeft);
  }
  placeDialogBars(content, buttons_,
                  PlacedDialog{.layout = layout, .topLeft = panelTopLeft});
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
  content.panel = tabbedPanelLayoutOf(metrics_).panel;
  content.tabs = std::move(tabStrip_);
  content.openTab = std::move(tabOpener_);
  content.nextKeys = PANEL_NEXT_KEYS;
  content.previousKeys = PANEL_PREVIOUS_KEYS;
}

PanelCursor MenuBuilder::placeRowsAndBars(Menu::Content& content) {
  TabbedPanelLayout layout = tabbedPanelLayoutOf(metrics_);
  layout.barSize.x = barWidthFor(buttons_, layout.barSize.x, metrics_);
  std::vector<PendingButton> trailing =
      barsAfterCancel(buttons_, cancelButton_);
  const PanelCursor end =
      placeRowsAndBarsOf(content, buttons_,
                         RowsAndBars{.firstRow = layout.firstRowTopLeft,
                                     .rowSize = layout.rowSize,
                                     .firstBar = {SAFE_MARGIN, layout.barsTop},
                                     .barSize = layout.barSize,
                                     .barGap = SPACE_BESIDE_PANEL_BARS});
  placeBarsAtPanelEnd(content, trailing, layout);
  content.rowScroll = rowScrollOf(content, layout);
  return end;
}

void MenuBuilder::placeDescriptionAndIllustration(Menu::Content& content,
                                                  PanelCursor end) {
  const TabbedPanelLayout layout = tabbedPanelLayoutOf(metrics_);
  content.descriptionAnchor = descriptionAnchorOf(layout, end, metrics_);
  if (metrics_ != TextMetrics{}) {
    content.descriptionWidth = layout.panel.position.x + layout.panel.size.x -
                               content.descriptionAnchor->x;
  }
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
  const int width = BUTTON_TEXT_OFFSET.x +
                    regularTextWidth(text.size, text.label) + CHIP_KEY_CAP_GAP +
                    KeyCap(text.keyName, KeyCap::Edge::Plain).width(text.size) +
                    BAR_KEY_CAP_INSET;
  const sf::Vector2i bottomRight =
      INTERFACE_AREA.size - sf::Vector2i(SAFE_MARGIN, SAFE_MARGIN);
  return {{right.value_or(bottomRight.x) - width, bottomRight.y - CHIP_HEIGHT},
          {width, CHIP_HEIGHT}};
}

}  // namespace rtype::client
