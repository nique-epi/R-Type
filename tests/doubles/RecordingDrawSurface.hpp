#pragma once

#include <SFML/Graphics/Color.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "SpriteDrawing.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "TextStyle.hpp"

/** @brief One rectangle a RecordingDrawSurface was asked to draw. */
struct DrawnImage {
  std::string assetId;
  rtype::game::SpriteFrame frame{};
  rtype::game::Position center{};
  rtype::client::SpriteTone tone{};
  std::optional<sf::Color> outline;
};

/** @brief One line a RecordingDrawSurface was asked to write. */
struct WrittenLine {
  std::string text;
  rtype::game::Position center{};
  rtype::client::TextStyle style;
};

/** @brief Draw surface double that writes down every rectangle it is asked to
 * draw and every line it is asked to write, each in order; it needs no
 * graphics card. */
class RecordingDrawSurface : public rtype::client::IDrawSurface {
 public:
  void draw(const rtype::client::SpriteDrawing& drawing) override;
  void write(std::string_view text, rtype::game::Position center,
             rtype::client::TextStyle style) override;

  [[nodiscard]] const std::vector<DrawnImage>& drawn() const;
  [[nodiscard]] const std::vector<WrittenLine>& written() const;
  [[nodiscard]] std::vector<std::string> assetIds() const;
  void forget();

 private:
  std::vector<DrawnImage> drawn_;
  std::vector<WrittenLine> written_;
};
