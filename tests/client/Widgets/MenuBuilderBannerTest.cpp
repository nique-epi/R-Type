#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "MenuBuilder.hpp"
#include "MenuBuilderBannerTestConstants.hpp"
#include "MenuPainting.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::MenuBuilder;
using rtype::client::testing::BANNER_UNDER_THE_LOGO;
using rtype::client::testing::LOGO_OVER_THE_BANNER;
using rtype::client::testing::SIZE_OF_THE_BANNER;
using rtype::client::testing::SIZE_OF_THE_LOGO_OVER_THE_BANNER;
using rtype::client::testing::SUBTITLE_UNDER_THE_BANNER;
using rtype::client::testing::VERSION_OVER_THE_BANNER;
using rtype::client::testing::WHERE_THE_BANNER_LIES;
using rtype::client::testing::WHERE_THE_SUBTITLE_FOLLOWS_THE_BANNER;

/**
 * Given a page with a version, a logo and a banner
 * When it is painted
 * Then the banner is drawn after the logo, one row under its bottom
 */
TEST(MenuBuilderBanner, TheBannerLiesOneRowUnderTheLogo) {
  const InterfaceCanvasRecord painting = paintingOf(
      MenuBuilder(MenuBuilder::Frame::Page)
          .version(std::string{VERSION_OVER_THE_BANNER})
          .logo(std::string{LOGO_OVER_THE_BANNER},
                SIZE_OF_THE_LOGO_OVER_THE_BANNER)
          .banner(std::string{BANNER_UNDER_THE_LOGO}, SIZE_OF_THE_BANNER)
          .build());

  ASSERT_EQ(painting.images.size(), 2U);
  EXPECT_EQ(painting.images.back().assetId, BANNER_UNDER_THE_LOGO);
  EXPECT_EQ(painting.images.back().topLeft, WHERE_THE_BANNER_LIES);
}

/**
 * Given a page with a version, a logo, a banner and a subtitle
 * When it is painted
 * Then the subtitle is written four rows under the banner
 */
TEST(MenuBuilderBanner, TheSubtitleFollowsTheBanner) {
  const InterfaceCanvasRecord painting = paintingOf(
      MenuBuilder(MenuBuilder::Frame::Page)
          .version(std::string{VERSION_OVER_THE_BANNER})
          .logo(std::string{LOGO_OVER_THE_BANNER},
                SIZE_OF_THE_LOGO_OVER_THE_BANNER)
          .banner(std::string{BANNER_UNDER_THE_LOGO}, SIZE_OF_THE_BANNER)
          .subtitle(std::string{SUBTITLE_UNDER_THE_BANNER})
          .build());

  EXPECT_EQ(anchorOf(painting, SUBTITLE_UNDER_THE_BANNER),
            WHERE_THE_SUBTITLE_FOLLOWS_THE_BANNER);
}

/**
 * Given a page asked for a banner but no logo
 * When it is painted
 * Then it draws no image
 */
TEST(MenuBuilderBanner, APageWithoutALogoShowsNoBanner) {
  const InterfaceCanvasRecord painting = paintingOf(
      MenuBuilder(MenuBuilder::Frame::Page)
          .banner(std::string{BANNER_UNDER_THE_LOGO}, SIZE_OF_THE_BANNER)
          .build());

  EXPECT_TRUE(painting.images.empty());
}
