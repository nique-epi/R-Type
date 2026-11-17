#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "TabRailTab.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A row of text tabs over a screen, the Tab Rail of the design system:
 * no box and no line, the open tab underlined in cyan, a locked tab in blue
 * with a SOON badge (TabRailConstants.hpp).
 *
 * The row is centred on a point and does not decide what a tab opens: the
 * screen that holds it asks tabAt() where the pointer is, and calls refuse()
 * when a locked tab is asked for, which shakes it.
 */
class TabRail {
 public:
  /**
   * @param tabs The tabs, left to right, at least one.
   * @param openTab Order of the open tab among them, never a locked one.
   * @param topMiddle Middle of the top edge of the row: the top of the labels.
   */
  TabRail(std::vector<TabRailTab> tabs, std::size_t openTab,
          sf::Vector2i topMiddle);

  void paint(IInterfaceCanvas& canvas) const;

  /** @returns The tab under a pixel of the interface, nothing between two
   * tabs or off the row. */
  [[nodiscard]] std::optional<std::size_t> tabAt(sf::Vector2i pixel) const;

  /** @returns Whether a tab is locked. */
  [[nodiscard]] bool isLocked(std::size_t tab) const;

  /** @brief Lights the tab under the pointer, nothing when the pointer left
   * the row. */
  void hover(std::optional<std::size_t> tab);

  /** @brief Shakes a locked tab for TAB_RAIL_REFUSAL_TIME. */
  void refuse(std::size_t tab);

  /** @brief Moves the shake of the refused tab on. */
  void advance(engine::Duration elapsed);

 private:
  /** @returns How far a tab is moved by its shake, in pixels. */
  [[nodiscard]] int shiftOf(std::size_t tab) const;
  void paintTab(IInterfaceCanvas& canvas, std::size_t tab) const;
  void paintIndicator(IInterfaceCanvas& canvas) const;

  std::vector<TabRailTab> tabs_;
  /** @brief Where the label of each tab and its badge lie, unshaken, from
   * the row its labels and their gaps make, centred on topMiddle_. */
  std::vector<sf::IntRect> areas_;
  std::size_t openTab_;
  sf::Vector2i topMiddle_;
  std::optional<std::size_t> hovered_;
  std::optional<std::size_t> refused_;
  engine::Duration sinceRefusal_{};
};

}  // namespace rtype::client
