#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <utility>
#include "AllEntityTypes.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"

using rtype::client::ENTITY_SHEET_ID;
using rtype::client::spriteOf;
using rtype::game::EntityType;
using rtype::game::Layer;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

namespace {

constexpr auto FIRST_VALUE_AFTER_THE_LAST_TYPE = static_cast<EntityType>(
    static_cast<std::uint8_t>(EntityType::EnemyMissile) + 1);

constexpr const char* SOURCE_ASSETS_FOLDER = RTYPE_SOURCE_ASSETS_FOLDER;

constexpr std::uint8_t TRANSPARENT_ALPHA = 0;

/** @brief Stands for the frame of a type without a sprite: it lies nowhere on
 * the sheet, and EveryEntityTypeHasASpriteOfTheEntitySheet names the type. */
constexpr SpriteFrame NO_FRAME{.left = 0, .top = 0, .width = 0, .height = 0};

SpriteFrame frameOf(EntityType type) {
  const std::optional<Sprite> sprite = spriteOf(type);
  return sprite.has_value() ? sprite->frame : NO_FRAME;
}

sf::Image shippedEntitySheet() {
  return sf::Image(std::filesystem::path(SOURCE_ASSETS_FOLDER) /
                   ENTITY_SHEET_ID);
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

}  // namespace

/**
 * Given every entity type the server can announce
 * When each one is looked up in the entity table
 * Then each has a sprite cut from the entity sheet
 */
TEST(EntitySprites, EveryEntityTypeHasASpriteOfTheEntitySheet) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    const std::optional<Sprite> sprite = spriteOf(type);

    ASSERT_TRUE(sprite.has_value());
    EXPECT_EQ(sprite.value_or(Sprite{}).assetId, ENTITY_SHEET_ID);
  }
}

/**
 * Given every entity type
 * When each one is looked up in the entity table
 * Then each is drawn in the entities layer, in front of the background and
 * behind the effects
 */
TEST(EntitySprites, EntitiesAreDrawnInTheEntitiesLayer) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    const std::optional<Sprite> sprite = spriteOf(type);

    ASSERT_TRUE(sprite.has_value());
    EXPECT_EQ(sprite.value_or(Sprite{}).layer, Layer::Entities);
  }
}

/**
 * Given a value past the last entity type, as a corrupted type would carry
 * When it is looked up in the entity table
 * Then the table gives no sprite
 */
TEST(EntitySprites, AValueOutsideTheEntityTypesHasNoSprite) {
  EXPECT_FALSE(spriteOf(FIRST_VALUE_AFTER_THE_LAST_TYPE).has_value());
}

/**
 * Given every pair of distinct entity types
 * When their rectangles in the entity sheet are compared
 * Then no two overlap, so no two types show the same pixels
 */
TEST(EntitySprites, NoTwoEntityTypesShareAnyPartOfTheSheet) {
  for (std::size_t first = 0; first < ALL_ENTITY_TYPES.size(); ++first) {
    for (std::size_t second = first + 1; second < ALL_ENTITY_TYPES.size();
         ++second) {
      SCOPED_TRACE(first);
      SCOPED_TRACE(second);

      EXPECT_FALSE(overlap(frameOf(ALL_ENTITY_TYPES[first]),
                           frameOf(ALL_ENTITY_TYPES[second])));
    }
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the rectangle of each entity type is laid on it
 * Then the rectangle lies inside the sheet and shows at least one visible pixel
 */
TEST(EntitySprites, EveryRectangleShowsPartOfTheShippedSheet) {
  const sf::Image sheet = shippedEntitySheet();
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));
    const SpriteFrame frame = frameOf(type);

    ASSERT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the pixels just outside the rectangle of each entity type are read
 * Then they are all transparent, so the rectangle neither cuts its sprite where
 * the sprite reaches the edge of its cell nor shows a neighbor
 */
TEST(EntitySprites, NoRectangleCutsItsSprite) {
  const sf::Image sheet = shippedEntitySheet();
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frameOf(type)));
  }
}
