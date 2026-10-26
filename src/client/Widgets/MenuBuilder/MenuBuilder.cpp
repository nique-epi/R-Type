#include "MenuBuilder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Button.hpp"
#include "IScreenNavigator.hpp"
#include "InterfaceConstants.hpp"
#include "KeyCap.hpp"
#include "Menu.hpp"
#include "RowControl.hpp"
#include "TabStrip.hpp"
#include "TextField.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

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
  const int left = SAFE_MARGIN;
  int top = SAFE_MARGIN;
  if (version_.has_value()) {
    content.labels.emplace_back(std::move(*version_), sf::Vector2i(left, top),
                                VERSION_STYLE);
    top += TEXT_LINE_HEIGHT + SPACE_BELOW_VERSION;
  }
  if (logo_.has_value()) {
    logo_->area.position = {left, top};
    top += logo_->area.size.y + SPACE_BELOW_HEADING;
    content.picture = std::move(logo_);
  }
  if (heading_.has_value()) {
    content.labels.emplace_back(std::move(*heading_), sf::Vector2i(left, top),
                                PAGE_LINE_STYLE);
    top += TEXT_LINE_HEIGHT + SPACE_BELOW_HEADING;
  }
  if (subtitle_.has_value()) {
    content.labels.emplace_back(std::move(*subtitle_), sf::Vector2i(left, top),
                                PAGE_LINE_STYLE);
    top += TEXT_LINE_HEIGHT + SPACE_BELOW_SUBTITLE;
  }
  content.rule = sf::IntRect({left, top}, RULE_SIZE);
  top += RULE_SIZE.y + SPACE_BELOW_RULE;
  for (PendingButton& pending : buttons_) {
    const sf::Vector2i size =
        pending.field.has_value() ? FIELD_SIZE : BUTTON_SIZE;
    content.buttons.emplace_back(
        std::move(pending.text), sf::IntRect({left, top}, size),
        std::move(pending.action), std::move(pending.field));
    top += size.y + BUTTON_GAP;
  }
  content.descriptionAnchor = sf::Vector2i(left, top);
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
  const sf::Vector2i panelTopLeft =
      (INTERFACE_AREA.size - DIALOG_SIZE) / SIDES_PER_AXIS;
  content.panel = sf::IntRect(panelTopLeft, DIALOG_SIZE);
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
  const int count = static_cast<int>(buttons_.size());
  const int rowWidth =
      barRowWidth(count, DIALOG_BUTTON_SIZE, DIALOG_BUTTON_GAP);
  int left = panelTopLeft.x + ((DIALOG_SIZE.x - rowWidth) / SIDES_PER_AXIS);
  for (PendingButton& pending : buttons_) {
    content.buttons.emplace_back(
        std::move(pending.text),
        sf::IntRect({left, panelTopLeft.y + DIALOG_BUTTONS_TOP},
                    DIALOG_BUTTON_SIZE),
        std::move(pending.action), std::move(pending.field));
    left += DIALOG_BUTTON_SIZE.x + DIALOG_BUTTON_GAP;
  }
}

void MenuBuilder::placeForm(Menu::Content& content) {
  const int fieldCount = static_cast<int>(std::ranges::count_if(
      buttons_, [](const PendingButton& pending) noexcept {
        return pending.field.has_value();
      }));
  const int buttonCount = static_cast<int>(buttons_.size()) - fieldCount;
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
  const sf::Vector2i panelTopLeft =
      (INTERFACE_AREA.size - panelSize) / SIDES_PER_AXIS;
  content.panel = sf::IntRect(panelTopLeft, panelSize);
  if (heading_.has_value()) {
    content.labels.emplace_back(std::move(*heading_),
                                panelTopLeft + FORM_HEADING_OFFSET,
                                DIALOG_HEADING_STYLE);
  }
  const int left = panelTopLeft.x + FORM_HEADING_OFFSET.x;
  const int rowWidth =
      barRowWidth(buttonCount, FORM_BUTTON_SIZE, FORM_BUTTON_GAP);
  int fieldTop = panelTopLeft.y + fieldsTop;
  int buttonLeft = panelTopLeft.x + ((FORM_WIDTH - rowWidth) / SIDES_PER_AXIS);
  for (PendingButton& pending : buttons_) {
    sf::IntRect area;
    if (pending.field.has_value()) {
      area = sf::IntRect({left, fieldTop}, FIELD_SIZE);
      fieldTop += FIELD_SIZE.y + SPACE_BETWEEN_FIELDS;
    } else {
      area = sf::IntRect({buttonLeft, panelTopLeft.y + buttonsTop},
                         FORM_BUTTON_SIZE);
      buttonLeft += FORM_BUTTON_SIZE.x + FORM_BUTTON_GAP;
    }
    content.buttons.emplace_back(std::move(pending.text), area,
                                 std::move(pending.action),
                                 std::move(pending.field));
  }
  content.descriptionAnchor =
      sf::Vector2i(left, panelTopLeft.y + descriptionTop);
  content.descriptionStyle = FORM_DESCRIPTION_STYLE;
  content.selectsOnHover = false;
  content.statusAnchor = sf::Vector2i(left, panelTopLeft.y + statusTop);
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

MenuBuilder::PanelCursor MenuBuilder::placeRowsAndBars(Menu::Content& content) {
  PanelCursor cursor{.rowTop = FIRST_ROW_TOP_LEFT.y, .barLeft = SAFE_MARGIN};
  for (PendingButton& pending : buttons_) {
    if (pending.control != nullptr) {
      content.buttons.emplace_back(
          std::move(pending.text),
          sf::IntRect({FIRST_ROW_TOP_LEFT.x, cursor.rowTop}, ROW_SIZE),
          std::move(pending.control));
      cursor.rowTop += ROW_SIZE.y + ROW_GAP;
    } else {
      content.buttons.emplace_back(
          std::move(pending.text),
          sf::IntRect({cursor.barLeft, PANEL_BARS_TOP}, PANEL_BAR_SIZE),
          std::move(pending.action), std::move(pending.field));
      cursor.barLeft += PANEL_BAR_SIZE.x + SPACE_BESIDE_PANEL_BARS;
    }
  }
  return cursor;
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

sf::IntRect MenuBuilder::chipArea(const Button::Text& text) {
  const int width =
      BUTTON_TEXT_OFFSET.x + regularTextWidth(text.label) + CHIP_KEY_CAP_GAP +
      KeyCap(text.keyName, KeyCap::Edge::Plain).width() + BAR_KEY_CAP_INSET;
  const sf::Vector2i bottomRight =
      INTERFACE_AREA.size - sf::Vector2i(SAFE_MARGIN, SAFE_MARGIN);
  return {{bottomRight.x - width, bottomRight.y - CHIP_HEIGHT},
          {width, CHIP_HEIGHT}};
}

}  // namespace rtype::client
