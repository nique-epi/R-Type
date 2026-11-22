#include "TabRail.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "TabRailConstants.hpp"
#include "TabRailTab.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns How wide a tab is: its label, and its badge when it is locked. */
int tabWidthOf(const TabRailTab& tab) {
  const int labelWidth = regularTextWidth(tab.label);
  return tab.locked ? labelWidth + TAB_RAIL_BADGE_GAP + TAB_RAIL_BADGE_SIZE.x
                    : labelWidth;
}

/** @returns How wide the whole row is, its gaps included. */
int rowWidthOf(const std::vector<TabRailTab>& tabs) {
  int width = 0;
  for (const TabRailTab& tab : tabs) {
    width += tabWidthOf(tab) + TAB_RAIL_GAP;
  }
  return width - TAB_RAIL_GAP;
}

/** @returns Where the label of each tab and its badge lie, the row centred
 * on @p topMiddle, each answering from TAB_RAIL_HIT_MARGIN over its label to
 * the bottom of the bar. */
std::vector<sf::IntRect> areasOf(const std::vector<TabRailTab>& tabs,
                                 sf::Vector2i topMiddle) {
  std::vector<sf::IntRect> areas;
  areas.reserve(tabs.size());
  int left = topMiddle.x - (rowWidthOf(tabs) / 2);
  for (const TabRailTab& tab : tabs) {
    const int width = tabWidthOf(tab);
    areas.push_back({{left, topMiddle.y - TAB_RAIL_HIT_MARGIN},
                     {width, TAB_RAIL_HIT_MARGIN + TAB_RAIL_INDICATOR_TOP +
                                 TAB_RAIL_INDICATOR_HEIGHT}});
    left += width + TAB_RAIL_GAP;
  }
  return areas;
}

/** @brief Paints the SOON badge of a locked tab, its box starting at
 * @p left. */
void paintBadge(IInterfaceCanvas& canvas, int left, int labelTop) {
  const sf::IntRect box{{left, labelTop - TAB_RAIL_BADGE_RISE},
                        TAB_RAIL_BADGE_SIZE};
  canvas.fill(box, TAB_RAIL_BADGE_COLOR);
  canvas.write(TAB_RAIL_BADGE_TEXT.in(canvas.language()),
               {box.position.x + (box.size.x / 2),
                box.position.y + TAB_RAIL_BADGE_TEXT_DROP},
               {.font = TextFont::Small,
                .color = TAB_RAIL_BADGE_TEXT_COLOR,
                .alignment = TextAlignment::Center});
}

}  // namespace

TabRail::TabRail(std::vector<TabRailTab> tabs, std::size_t openTab,
                 sf::Vector2i topMiddle)
    : tabs_(std::move(tabs)),
      areas_(areasOf(tabs_, topMiddle)),
      openTab_(openTab),
      topMiddle_(topMiddle) {}

void TabRail::paint(IInterfaceCanvas& canvas) const {
  for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
    paintTab(canvas, tab);
  }
  paintIndicator(canvas);
}

std::optional<std::size_t> TabRail::tabAt(sf::Vector2i pixel) const {
  for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
    if (areas_[tab].contains(pixel)) {
      return tab;
    }
  }
  return std::nullopt;
}

bool TabRail::isLocked(std::size_t tab) const { return tabs_.at(tab).locked; }

void TabRail::hover(std::optional<std::size_t> tab) { hovered_ = tab; }

void TabRail::refuse(std::size_t tab) {
  refused_ = tab;
  sinceRefusal_ = engine::Duration::zero();
}

void TabRail::advance(engine::Duration elapsed) {
  if (!refused_.has_value()) {
    return;
  }
  sinceRefusal_ += elapsed;
  if (sinceRefusal_ >= TAB_RAIL_REFUSAL_TIME) {
    refused_.reset();
  }
}

int TabRail::shiftOf(std::size_t tab) const {
  if (refused_ != tab) {
    return 0;
  }
  const auto stepTime =
      std::chrono::duration_cast<engine::Duration>(TAB_RAIL_REFUSAL_TIME) /
      TAB_RAIL_REFUSAL_STEPS;
  const auto step = sinceRefusal_ / stepTime;
  return step % 2 == 0 ? -TAB_RAIL_REFUSAL_SHIFT : TAB_RAIL_REFUSAL_SHIFT;
}

void TabRail::paintTab(IInterfaceCanvas& canvas, std::size_t tab) const {
  const TabRailTab& shown = tabs_[tab];
  const int left = areas_[tab].position.x + shiftOf(tab);
  sf::Color color = TAB_RAIL_IDLE_COLOR;
  if (shown.locked) {
    color = TAB_RAIL_LOCKED_COLOR;
  } else if (tab == openTab_ || hovered_ == tab) {
    color = TAB_RAIL_OPEN_COLOR;
  }
  canvas.write(shown.label, {left, topMiddle_.y},
               {.font = TextFont::Regular, .color = color, .outlined = true});
  if (shown.locked) {
    paintBadge(canvas,
               left + regularTextWidth(shown.label) + TAB_RAIL_BADGE_GAP,
               topMiddle_.y);
  }
}

void TabRail::paintIndicator(IInterfaceCanvas& canvas) const {
  const int left = areas_[openTab_].position.x;
  const int labelWidth = regularTextWidth(tabs_[openTab_].label);
  canvas.fill(
      {{left + TAB_RAIL_INDICATOR_INSET, topMiddle_.y + TAB_RAIL_INDICATOR_TOP},
       {labelWidth - (2 * TAB_RAIL_INDICATOR_INSET),
        TAB_RAIL_INDICATOR_HEIGHT}},
      TAB_RAIL_INDICATOR_COLOR);
  canvas.fill({{left + ((labelWidth - TAB_RAIL_DOT_SIZE) / 2),
                topMiddle_.y + TAB_RAIL_DOT_TOP},
               {TAB_RAIL_DOT_SIZE, TAB_RAIL_DOT_SIZE}},
              TAB_RAIL_INDICATOR_COLOR);
}

}  // namespace rtype::client
