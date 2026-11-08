#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"
#include "MetalPatch.hpp"
#include "MetalTexture.hpp"
#include "MetalTextureTestConstants.hpp"

using rtype::client::addBrushStrokes;
using rtype::client::addWear;
using rtype::client::ColoredPixel;
using rtype::client::MetalPatch;
using rtype::client::testing::ANY_SEED;
using rtype::client::testing::DARK_SHADE;
using rtype::client::testing::LARGE_PLATE;
using rtype::client::testing::LIGHT_SHADE;
using rtype::client::testing::SIX_BY_SIX;

/**
 * Given the locker's large plate, worn and brushed
 * When every pixel of its texture is listed
 * Then each lies on the plate: no scratch or stroke runs off its sides
 */
TEST(MetalTexture, ALargePlateKeepsItsTextureInside) {
  std::vector<ColoredPixel> pixels;
  const MetalPatch patch{.area = LARGE_PLATE,
                         .shades = {.light = LIGHT_SHADE, .dark = DARK_SHADE},
                         .seed = ANY_SEED};

  addBrushStrokes(pixels, patch);
  addWear(pixels, patch);

  ASSERT_FALSE(pixels.empty());
  for (const ColoredPixel& pixel : pixels) {
    EXPECT_TRUE(LARGE_PLATE.contains(pixel.position))
        << pixel.position.x << ", " << pixel.position.y;
  }
}

/**
 * Given a plate six pixels wide, the edge case of the scratches
 * When its wear is added
 * Then no pixel lies off the plate
 */
TEST(MetalTexture, ASmallPlateKeepsItsWearInside) {
  std::vector<ColoredPixel> pixels;

  addWear(pixels,
          MetalPatch{.area = SIX_BY_SIX,
                     .shades = {.light = LIGHT_SHADE, .dark = DARK_SHADE},
                     .seed = ANY_SEED});

  for (const ColoredPixel& pixel : pixels) {
    EXPECT_TRUE(SIX_BY_SIX.contains(pixel.position))
        << pixel.position.x << ", " << pixel.position.y;
  }
}

/**
 * Given the same plate textured twice with the same seed
 * When both textures are compared
 * Then they are the same pixels: the locker does not flicker between frames
 */
TEST(MetalTexture, TheSameSeedGivesTheSameTexture) {
  const MetalPatch patch{.area = LARGE_PLATE,
                         .shades = {.light = LIGHT_SHADE, .dark = DARK_SHADE},
                         .seed = ANY_SEED};
  std::vector<ColoredPixel> first;
  std::vector<ColoredPixel> second;

  addWear(first, patch);
  addWear(second, patch);

  ASSERT_EQ(first.size(), second.size());
  for (std::size_t index = 0; index < first.size(); ++index) {
    EXPECT_EQ(first.at(index).position, second.at(index).position);
  }
}
