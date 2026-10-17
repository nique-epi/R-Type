#include "TabStrip.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyCap.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

TabStrip::TabStrip(std::vector<std::string> labels, std::size_t openTab,
                   KeyNames keys, sf::Vector2i topLeft)
    : labels_(std::move(labels)),
      openTab_(openTab),
      keys_(std::move(keys)),
      topLeft_(topLeft) {}

void TabStrip::paint(IInterfaceCanvas& canvas) const {
  KeyCap(keys_.previous, KeyCap::Edge::Plain)
      .paint(canvas, previousKeyArea().position);
  KeyCap(keys_.next, KeyCap::Edge::Plain).paint(canvas, nextKeyArea().position);
  for (std::size_t tab = 0; tab < labels_.size(); ++tab) {
    if (tab != openTab_) {
      paintTab(canvas, tab);
    }
  }
  paintTab(canvas, openTab_);
}

std::optional<std::size_t> TabStrip::tabOpenedAt(sf::Vector2i pixel) const {
  if (previousKeyArea().contains(pixel)) {
    return previousTab();
  }
  if (nextKeyArea().contains(pixel)) {
    return nextTab();
  }
  for (std::size_t tab = 0; tab < labels_.size(); ++tab) {
    if (tabArea(tab).contains(pixel)) {
      return tab;
    }
  }
  return std::nullopt;
}

std::size_t TabStrip::previousTab() const {
  return (openTab_ + labels_.size() - 1) % labels_.size();
}

std::size_t TabStrip::nextTab() const {
  return (openTab_ + 1) % labels_.size();
}

sf::IntRect TabStrip::previousKeyArea() const {
  return {{topLeft_.x, topLeft_.y + TAB_KEY_CAP_TOP},
          {KEY_CAP_MINIMUM_WIDTH, CONTROL_HEIGHT}};
}

sf::IntRect TabStrip::nextKeyArea() const {
  return {{topLeft_.x + TAB_STRIP_WIDTH - KEY_CAP_MINIMUM_WIDTH,
           topLeft_.y + TAB_KEY_CAP_TOP},
          {KEY_CAP_MINIMUM_WIDTH, CONTROL_HEIGHT}};
}

sf::IntRect TabStrip::tabArea(std::size_t tab) const {
  const int count = static_cast<int>(labels_.size());
  const int firstLeft = KEY_CAP_MINIMUM_WIDTH + TAB_GAP;
  const int width = (TAB_STRIP_WIDTH - (SIDES_PER_AXIS * firstLeft) -
                     ((count - 1) * TAB_GAP)) /
                    count;
  const int left =
      topLeft_.x + firstLeft + (static_cast<int>(tab) * (width + TAB_GAP));
  return {{left, topLeft_.y}, {width, TAB_HEIGHT}};
}

void TabStrip::paintTab(IInterfaceCanvas& canvas, std::size_t tab) const {
  const bool open = tab == openTab_;
  sf::IntRect area = tabArea(tab);
  if (open) {
    area.size.y += BORDER_THICKNESS;
  }
  CutCornerBox(area,
               {.border = open ? OPEN_TAB_BORDER_COLOR : TAB_BORDER_COLOR,
                .inside = TAB_INSIDE_COLOR},
               CutCornerBox::Shape::OpenBottom)
      .paint(canvas);
  const std::string& label = labels_[tab];
  canvas.write(label,
               {centeredTextLeft(area, label), area.position.y + TAB_LABEL_TOP},
               {.font = TextFont::Regular,
                .color = open ? OPEN_TAB_TEXT_COLOR : TAB_TEXT_COLOR});
}

}  // namespace rtype::client
