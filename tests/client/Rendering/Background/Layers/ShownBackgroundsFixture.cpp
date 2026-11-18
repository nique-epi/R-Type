#include "ShownBackgroundsFixture.hpp"
#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundCatalogFixture.hpp"
#include "BackgroundDescription.hpp"
#include "LayeredBackgroundFixture.hpp"
#include "ShownBackgrounds.hpp"
#include "ShownBackgroundsTestConstants.hpp"
#include "SkyLevel.hpp"

using rtype::client::BackgroundCatalog;
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

std::shared_ptr<const BackgroundCatalog> ShownBackgroundsFixture::packOf(
    std::string_view file, std::string_view text) const {
  pack_.write(file, text);
  const std::array folders{std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER},
                           pack_.path()};
  return std::make_shared<const BackgroundCatalog>(
      BackgroundCatalog::ofLastFolder(folders));
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
