#include "MenuBuilder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <string>
#include <utility>
#include "ButtonText.hpp"
#include "IScreenNavigator.hpp"
#include "InterfaceConstants.hpp"
#include "Menu.hpp"
#include "MenuContent.hpp"
#include "MenuFrame.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

namespace rtype::client {

MenuBuilder::MenuBuilder(MenuFrame frame) : frame_(frame) {}

MenuBuilder& MenuBuilder::version(std::string text) {
  version_ = std::move(text);
  return *this;
}

MenuBuilder& MenuBuilder::logo(std::string assetId, sf::Vector2i size) {
  logo_ = MenuPicture{.assetId = std::move(assetId), .area = {{0, 0}, size}};
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

MenuBuilder& MenuBuilder::button(ButtonText text,
                                 std::function<void()> action) {
  buttons_.push_back({.text = std::move(text), .action = std::move(action)});
  return *this;
}

MenuBuilder& MenuBuilder::cancelButton(ButtonText text,
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
  MenuContent content;
  content.cancelButton = cancelButton_;
  switch (frame_) {
    case MenuFrame::Dialog:
      content.dimmed = true;
      placeDialog(content);
      break;
    case MenuFrame::Page:
      placeColumn(content);
      break;
  }
  return Menu(std::move(content));
}

void MenuBuilder::placeColumn(MenuContent& content) {
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
    content.buttons.emplace_back(std::move(pending.text),
                                 sf::IntRect({left, top}, BUTTON_SIZE),
                                 std::move(pending.action));
    top += BUTTON_SIZE.y + BUTTON_GAP;
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

void MenuBuilder::placeDialog(MenuContent& content) {
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
        std::move(pending.action));
    left += DIALOG_BUTTON_SIZE.x + DIALOG_BUTTON_GAP;
  }
}

}  // namespace rtype::client
