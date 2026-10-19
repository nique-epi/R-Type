#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"

/** @brief One rectangle a RecordingDrawSurface was asked to draw. */
struct DrawnImage {
  std::string assetId;
  rtype::game::SpriteFrame frame{};
  rtype::game::Position center{};
  rtype::client::SpriteTone tone{};
};

/** @brief Draw surface double that writes down every rectangle it is asked to
 * draw, in order, with its tone; it needs no graphics card. */
class RecordingDrawSurface : public rtype::client::IDrawSurface {
 public:
  void draw(std::string_view assetId, rtype::game::SpriteFrame frame,
            rtype::game::Position center,
            rtype::client::SpriteTone tone) override;

  [[nodiscard]] const std::vector<DrawnImage>& drawn() const;
  [[nodiscard]] std::vector<std::string> assetIds() const;
  void forget();

 private:
  std::vector<DrawnImage> drawn_;
};
