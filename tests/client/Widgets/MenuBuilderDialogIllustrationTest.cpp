#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuBuilderDialogIllustrationTestConstants.hpp"
#include "MenuPainting.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::testing::BACK_OF_THE_PICTURE;
using rtype::client::testing::BACK_UNDER_THE_PICTURE;
using rtype::client::testing::DIALOG_PICTURE_AREA;
using rtype::client::testing::DIALOG_PICTURE_SIZE;

/**
 * Given a dialog holding an illustration and BACK
 * When it is built
 * Then its illustration lies where its heading goes
 */
TEST(MenuBuilderDialogIllustration, TheIllustrationLiesWhereTheHeadingGoes) {
  const Menu menu =
      MenuBuilder(MenuBuilder::Frame::Dialog)
          .illustration(DIALOG_PICTURE_SIZE)
          .cancelButton({.label = std::string{BACK_OF_THE_PICTURE}}, [] {})
          .build();

  EXPECT_EQ(menu.illustrationArea().value_or(sf::IntRect{}),
            DIALOG_PICTURE_AREA);
}

/**
 * Given a dialog holding an illustration and BACK
 * When it is painted
 * Then BACK stands under the illustration, the panel as tall as it needs
 */
TEST(MenuBuilderDialogIllustration, TheButtonsStandUnderTheIllustration) {
  const InterfaceCanvasRecord painting = paintingOf(
      MenuBuilder(MenuBuilder::Frame::Dialog)
          .illustration(DIALOG_PICTURE_SIZE)
          .cancelButton({.label = std::string{BACK_OF_THE_PICTURE}}, [] {})
          .build());

  EXPECT_EQ(anchorOf(painting, BACK_OF_THE_PICTURE), BACK_UNDER_THE_PICTURE);
}
