#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "DrawJournal.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

namespace sf {
class RenderTarget;
}  // namespace sf

/** @brief A rectangle filled on a recording canvas. */
struct RecordedFill {
  sf::IntRect area;
  sf::Color color;

  bool operator==(const RecordedFill& other) const = default;
};

/** @brief A line written on a recording canvas. */
struct RecordedWriting {
  std::string text;
  sf::Vector2i anchor;
  rtype::client::TextStyle style;
};

/** @brief An image drawn on a recording canvas. */
struct RecordedImage {
  std::string assetId;
  sf::IntRect source;
  sf::Vector2i topLeft;
  int scale{};
  sf::IntRect visibleArea;
};

/** @brief A texture laid smoothed on a recording canvas. */
struct RecordedSmoothImage {
  std::string assetId;
  sf::IntRect area;
  sf::Color tint;
};

/**
 * @brief What a RecordingInterfaceCanvas was asked since its last clear(), and
 * how many times it was displayed and shown, which clear() keeps; the test
 * keeps it, since the screen owns the canvas.
 */
struct InterfaceCanvasRecord {
  std::vector<RecordedFill> fills;
  std::vector<RecordedWriting> writings;
  std::vector<RecordedImage> images;
  std::vector<RecordedSmoothImage> smoothImages;
  int displayCount = 0;
  int showCount = 0;
  /** @brief Where the canvas writes INTERFACE_CANVAS_LAID each time it is
   * shown, when a test shares one with other doubles. */
  DrawJournal* drawJournal = nullptr;
};

/** @brief Advance a recording canvas gives each character of small print:
 * Silkscreen is proportional (3 to 7 pixels), so the double takes 6, the
 * width of most of its capitals; the regular font keeps its real
 * REGULAR_FONT_ADVANCE. */
constexpr int RECORDED_SMALL_ADVANCE = 6;

/** @brief Interface canvas double that writes down what it is asked; it needs
 * no graphics card. */
class RecordingInterfaceCanvas : public rtype::client::IInterfaceCanvas {
 public:
  /** @param record Where the calls are written down; it must outlive the
   * canvas. */
  explicit RecordingInterfaceCanvas(InterfaceCanvasRecord& record);

  void clear() override;
  void fill(sf::IntRect area, sf::Color color) override;
  void write(std::string_view text, sf::Vector2i anchor,
             rtype::client::TextStyle style) override;
  void drawImage(rtype::client::TextureArea image,
                 rtype::client::ImagePlacement placement,
                 sf::IntRect visibleArea) override;
  void drawSmoothImage(std::string_view assetId, sf::IntRect area,
                       sf::Color tint) override;
  [[nodiscard]] int textWidth(std::string_view text,
                              rtype::client::TextFont font) const override;
  void display() override;
  void show(sf::RenderTarget& target) const override;

 private:
  InterfaceCanvasRecord* record_;
};

/** @returns The first line written with this text since the last clear(), or
 * nullptr. */
[[nodiscard]] const RecordedWriting* writingOf(
    const InterfaceCanvasRecord& record, std::string_view text);

/** @returns Where the first line written with this text since the last
 * clear() was anchored, or nothing. */
[[nodiscard]] std::optional<sf::Vector2i> anchorOf(
    const InterfaceCanvasRecord& record, std::string_view text);

/** @returns Every pixel the fills of a record light, as (column, row). */
[[nodiscard]] std::set<std::pair<int, int>> litPixelsOf(
    const InterfaceCanvasRecord& record);
