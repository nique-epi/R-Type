#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief The tabs over a panel of rows, between the keycaps of the keys that
 * open the previous and the next tab.
 *
 * The strip is TAB_STRIP_WIDTH wide unless told otherwise, and TAB_HEIGHT tall;
 * its tabs share what the keycaps leave, TAB_GAP apart. The open tab has a cyan
 * border and no bottom edge, and is one pixel taller than the others: painted
 * after the panel, it covers the panel's border under it and opens into it. A
 * closed tab stands on that border.
 */
class TabStrip {
 public:
  /** @brief The names written on the two keycaps. */
  struct KeyNames {
    std::string previous;
    std::string next;
  };

  /**
   * @param labels Labels of the tabs, left to right, at least one.
   * @param openTab Order of the open tab among them.
   * @param topLeft Top-left corner of the strip.
   */
  TabStrip(std::vector<std::string> labels, std::size_t openTab, KeyNames keys,
           sf::Vector2i topLeft);

  /** @brief Sets how wide the strip is, TAB_STRIP_WIDTH until then, for a panel
   * narrower than the safe margin. */
  void setWidth(int width);

  /** @brief Sets the text size the strip is laid out at, as the fonts are
   * drawn until then: its tabs and keycaps grow with it, and when the tabs
   * no longer fit side by side each is as wide as its label and the strip
   * shows those up to the open one. */
  void setTextSize(TextMetrics metrics);

  void paint(IInterfaceCanvas& canvas) const;

  /** @returns The tab a click at a pixel opens: the tab under it, the
   * previous or next tab under a keycap, nothing elsewhere. */
  [[nodiscard]] std::optional<std::size_t> tabOpenedAt(
      sf::Vector2i pixel) const;

  /** @returns The tab before the open one and the one after it, wrapping
   * around. */
  [[nodiscard]] std::size_t previousTab() const;
  [[nodiscard]] std::size_t nextTab() const;

 private:
  /** @returns Where a tab lies, or nothing when the strip does not show it.
   */
  [[nodiscard]] std::optional<sf::IntRect> tabArea(std::size_t tab) const;
  [[nodiscard]] sf::IntRect previousKeyArea() const;
  [[nodiscard]] sf::IntRect nextKeyArea() const;
  void paintTab(IInterfaceCanvas& canvas, std::size_t tab) const;

  std::vector<std::string> labels_;
  std::size_t openTab_;
  KeyNames keys_;
  sf::Vector2i topLeft_;
  int width_ = TAB_STRIP_WIDTH;
  TextMetrics metrics_{};
};

}  // namespace rtype::client
