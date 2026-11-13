#include "TabStrip.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyCap.hpp"
#include "TabRun.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns The area of a keycap of the strip whose left side is @p left, as
 * wide as a keycap of one letter, centered on the height of a tab. */
sf::IntRect keyAreaAt(int left, TextMetrics metrics, int top) {
  const int height = textBoxHeight(CONTROL_HEIGHT, metrics);
  const int tab = textBoxHeight(TAB_HEIGHT, metrics);
  return {{left, top + ((tab - height) / SIDES_PER_AXIS)},
          {KeyCap(ONE_LETTER_KEY, KeyCap::Edge::Plain).width(metrics), height}};
}

/** @returns How wide each tab of @p labels is when it is as wide as its
 * label and TAB_TEXT_PADDING on each side. */
std::vector<int> tabWidthsOf(const std::vector<std::string>& labels,
                             TextMetrics metrics) {
  std::vector<int> widths;
  widths.reserve(labels.size());
  for (const std::string& label : labels) {
    widths.push_back(regularTextWidth(metrics, label) +
                     (SIDES_PER_AXIS * TAB_TEXT_PADDING));
  }
  return widths;
}

/** @returns How wide each tab is when the tabs share the room equally. */
int equalTabWidthOf(const TabRun& run) {
  const int count = static_cast<int>(run.widths.size());
  return (run.room - ((count - 1) * TAB_GAP)) / count;
}

/** @returns The first tab shown when the tabs, each as wide as its label, do
 * not all fit: the first one from which the tabs up to @p open fit. */
std::size_t firstShownTabOf(const TabRun& run, std::size_t open) {
  std::size_t first = 0;
  while (first < open) {
    int used = -TAB_GAP;
    for (std::size_t tab = first; tab <= open; ++tab) {
      used += run.widths[tab] + TAB_GAP;
    }
    if (used <= run.room) {
      return first;
    }
    ++first;
  }
  return open;
}

}  // namespace

TabStrip::TabStrip(std::vector<std::string> labels, std::size_t openTab,
                   KeyNames keys, sf::Vector2i topLeft)
    : labels_(std::move(labels)),
      openTab_(openTab),
      keys_(std::move(keys)),
      topLeft_(topLeft) {}

void TabStrip::setWidth(int width) { width_ = width; }

void TabStrip::setTextSize(TextMetrics metrics) { metrics_ = metrics; }

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
    const std::optional<sf::IntRect> area = tabArea(tab);
    if (area.has_value() && area->contains(pixel)) {
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
  return keyAreaAt(topLeft_.x, metrics_, topLeft_.y);
}

sf::IntRect TabStrip::nextKeyArea() const {
  const sf::IntRect area = keyAreaAt(topLeft_.x, metrics_, topLeft_.y);
  return {{topLeft_.x + width_ - area.size.x, area.position.y}, area.size};
}

std::optional<sf::IntRect> TabStrip::tabArea(std::size_t tab) const {
  const int firstLeft = previousKeyArea().size.x + TAB_GAP;
  const TabRun run{.widths = tabWidthsOf(labels_, metrics_),
                   .room = width_ - (SIDES_PER_AXIS * firstLeft)};
  const int equalWidth = equalTabWidthOf(run);
  const bool equal = std::ranges::all_of(
      run.widths, [equalWidth](int width) { return width <= equalWidth; });
  const std::size_t first = equal ? 0 : firstShownTabOf(run, openTab_);
  if (tab < first) {
    return std::nullopt;
  }
  int left = topLeft_.x + firstLeft;
  for (std::size_t before = first; before < tab; ++before) {
    left += (equal ? equalWidth : run.widths[before]) + TAB_GAP;
  }
  const int width = equal ? equalWidth : run.widths[tab];
  if (left + width > topLeft_.x + width_ - firstLeft + TAB_GAP) {
    return std::nullopt;
  }
  return sf::IntRect({left, topLeft_.y},
                     {width, textBoxHeight(TAB_HEIGHT, metrics_)});
}

void TabStrip::paintTab(IInterfaceCanvas& canvas, std::size_t tab) const {
  const bool open = tab == openTab_;
  const std::optional<sf::IntRect> shown = tabArea(tab);
  if (!shown.has_value()) {
    return;
  }
  sf::IntRect area = *shown;
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
               {centeredTextLeft(area, label, canvas.textMetrics()),
                area.position.y + TAB_LABEL_TOP},
               {.font = TextFont::Regular,
                .color = open ? OPEN_TAB_TEXT_COLOR : TAB_TEXT_COLOR});
}

}  // namespace rtype::client
