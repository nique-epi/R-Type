#include "MenuBuilder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "Button.hpp"
#include "IScreenNavigator.hpp"
#include "InterfaceConstants.hpp"
#include "Menu.hpp"
#include "TextField.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

namespace rtype::client {

MenuBuilder::MenuBuilder(Frame frame) : frame_(frame) {}

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
                      .field = std::nullopt});
  return *this;
}

MenuBuilder& MenuBuilder::textField(Button::Text text, TextField field,
                                    std::function<void()> action) {
  buttons_.push_back({.text = std::move(text),
                      .action = std::move(action),
                      .field = std::move(field)});
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

Menu MenuBuilder::build() {
  Menu::Content content;
  content.cancelButton = cancelButton_;
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
  }
  return Menu(std::move(content));
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
  content.descriptionAnchor = {left, top};
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
      (count * DIALOG_BUTTON_SIZE.x) + ((count - 1) * DIALOG_BUTTON_GAP);
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
  const int rowWidth = (buttonCount * FORM_BUTTON_SIZE.x) +
                       ((buttonCount - 1) * FORM_BUTTON_GAP);
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
  content.descriptionAnchor = {left, panelTopLeft.y + descriptionTop};
  content.descriptionStyle = FORM_DESCRIPTION_STYLE;
  content.selectsOnHover = false;
  content.statusAnchor = sf::Vector2i(left, panelTopLeft.y + statusTop);
}

}  // namespace rtype::client
