#include "SheetPixels.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <filesystem>
#include <span>
#include <string_view>
#include <utility>
#include "SheetPixelsTestConstants.hpp"
#include "SpriteFrame.hpp"

using rtype::game::SpriteFrame;

namespace {

bool isVisible(const sf::Image& image, int column, int row) {
  const sf::Vector2u size = image.getSize();
  if (column < 0 || row < 0 || std::cmp_greater_equal(column, size.x) ||
      std::cmp_greater_equal(row, size.y)) {
    return false;
  }
  const sf::Vector2u pixel{static_cast<unsigned int>(column),
                           static_cast<unsigned int>(row)};
  return image.getPixel(pixel).a != TRANSPARENT_ALPHA;
}

}  // namespace

sf::Image shippedSheet(std::string_view assetId) {
  return sf::Image(std::filesystem::path(RTYPE_SOURCE_ASSETS_FOLDER) /
                   std::filesystem::path(assetId));
}

bool overlap(SpriteFrame first, SpriteFrame second) {
  return first.left < second.left + second.width &&
         second.left < first.left + first.width &&
         first.top < second.top + second.height &&
         second.top < first.top + first.height;
}

bool liesInside(SpriteFrame frame, sf::Vector2u imageSize) {
  return frame.left >= 0 && frame.top >= 0 && frame.width > 0 &&
         frame.height > 0 &&
         std::cmp_less_equal(frame.left + frame.width, imageSize.x) &&
         std::cmp_less_equal(frame.top + frame.height, imageSize.y);
}

bool showsAVisiblePixel(const sf::Image& image, SpriteFrame frame) {
  for (int row = frame.top; row < frame.top + frame.height; ++row) {
    for (int column = frame.left; column < frame.left + frame.width; ++column) {
      if (isVisible(image, column, row)) {
        return true;
      }
    }
  }
  return false;
}

bool hasAVisiblePixelAround(const sf::Image& image, SpriteFrame frame) {
  const int right = frame.left + frame.width;
  const int bottom = frame.top + frame.height;
  for (int row = frame.top; row < bottom; ++row) {
    if (isVisible(image, frame.left - 1, row) || isVisible(image, right, row)) {
      return true;
    }
  }
  for (int column = frame.left; column < right; ++column) {
    if (isVisible(image, column, frame.top - 1) ||
        isVisible(image, column, bottom)) {
      return true;
    }
  }
  return false;
}

bool showsColor(const sf::Image& image, SpriteFrame frame, sf::Color color) {
  for (int row = frame.top; row < frame.top + frame.height; ++row) {
    for (int column = frame.left; column < frame.left + frame.width; ++column) {
      if (isVisible(image, column, row) &&
          image.getPixel({static_cast<unsigned int>(column),
                          static_cast<unsigned int>(row)}) == color) {
        return true;
      }
    }
  }
  return false;
}

int changedPixelCount(const sf::Image& image, SpriteFrame frame,
                      SpriteFrame other) {
  int changed = 0;
  for (int row = 0; row < frame.height; ++row) {
    for (int column = 0; column < frame.width; ++column) {
      const sf::Vector2u pixel{static_cast<unsigned int>(frame.left + column),
                               static_cast<unsigned int>(frame.top + row)};
      const sf::Vector2u otherPixel{
          static_cast<unsigned int>(other.left + column),
          static_cast<unsigned int>(other.top + row)};
      if (image.getPixel(pixel) != image.getPixel(otherPixel)) {
        ++changed;
      }
    }
  }
  return changed;
}

int mirrorMismatchCount(const sf::Image& image, SpriteFrame frame,
                        SpriteFrame mirror) {
  int mismatched = 0;
  for (int row = 0; row < frame.height; ++row) {
    for (int column = 0; column < frame.width; ++column) {
      const sf::Vector2u pixel{static_cast<unsigned int>(frame.left + column),
                               static_cast<unsigned int>(frame.top + row)};
      const sf::Vector2u mirroredPixel{
          static_cast<unsigned int>(mirror.left + mirror.width - 1 - column),
          static_cast<unsigned int>(mirror.top + row)};
      if (image.getPixel(pixel) != image.getPixel(mirroredPixel)) {
        ++mismatched;
      }
    }
  }
  return mismatched;
}

int visiblePixelCount(const sf::Image& image, SpriteFrame frame) {
  int visible = 0;
  for (int row = frame.top; row < frame.top + frame.height; ++row) {
    for (int column = frame.left; column < frame.left + frame.width; ++column) {
      if (isVisible(image, column, row)) {
        ++visible;
      }
    }
  }
  return visible;
}

int largestVisibleArea(const sf::Image& image,
                       std::span<const SpriteFrame> frames) {
  int largest = 0;
  for (const SpriteFrame& frame : frames) {
    largest = std::max(largest, visiblePixelCount(image, frame));
  }
  return largest;
}
