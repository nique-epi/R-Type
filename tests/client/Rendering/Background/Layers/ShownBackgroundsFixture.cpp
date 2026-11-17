#include "ShownBackgroundsFixture.hpp"
#include <string>
#include <string_view>
#include "BackgroundCatalogFixture.hpp"
#include "BackgroundDescription.hpp"
#include "LayeredBackgroundFixture.hpp"
#include "ShownBackgrounds.hpp"
#include "ShownBackgroundsTestConstants.hpp"
#include "SkyLevel.hpp"

using rtype::client::BackgroundDescription;
using rtype::client::ShownBackgrounds;
using rtype::client::SkyLevel;

ShownBackgroundsFixture::ShownBackgroundsFixture()
    : shipped_(shippedBackgrounds()) {}

ShownBackgrounds ShownBackgroundsFixture::shippedSky() const {
  return ShownBackgrounds{shipped_, shippedSources()};
}

const BackgroundDescription& ShownBackgroundsFixture::described(
    std::string_view name) const {
  const BackgroundDescription* const found = shipped_.named(name);
  return found == nullptr ? *shipped_.named(SHIPPED_SKY) : *found;
}

SkyLevel levelInThePit(float unitsIn) {
  return SkyLevel{.biome = std::string{SHIPPED_PIT},
                  .scrollPosition = WHERE_THE_PIT_BEGAN + unitsIn,
                  .scrollSpeed = 0.0F,
                  .biomeSince = WHERE_THE_PIT_BEGAN};
}

void passOneSecond(ShownBackgrounds& backgrounds) {
  for (int frame = 0; frame < FRAMES_OF_ONE_SECOND; ++frame) {
    backgrounds.advance(ONE_FRAME_OF_SKY);
  }
}
