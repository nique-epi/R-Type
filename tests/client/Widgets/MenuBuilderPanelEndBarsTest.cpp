#include <gtest/gtest.h>
#include "MenuBuilderPanelEndBarsFixtures.hpp"
#include "MenuBuilderPanelEndBarsTestConstants.hpp"
#include "MenuPainting.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingRowControl.hpp"

using rtype::client::testing::DESCRIPTION_RIGHT_OF_BACK;
using rtype::client::testing::LABEL_OF_THE_BAR_AT_THE_PANEL_END;

/**
 * Given a tabbed panel whose QUIT is asked for after BACK, its cancel button
 * When it is painted
 * Then QUIT ends at the right end of the panel, its label at (388, 230)
 */
TEST(MenuBuilderPanelEndBars, ABarAfterTheCancelButtonEndsAtThePanelEnd) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting =
      paintingOf(panelWithQuitAfterBack(record));

  EXPECT_EQ(anchorOf(painting, "QUIT"), LABEL_OF_THE_BAR_AT_THE_PANEL_END);
}

/**
 * Given the same panel, its row selected
 * When it is painted
 * Then the description of the row stays right of BACK, at (112, 230)
 */
TEST(MenuBuilderPanelEndBars, TheDescriptionStaysRightOfTheCancelButton) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting =
      paintingOf(panelWithQuitAfterBack(record));

  EXPECT_EQ(anchorOf(painting, "> SIZES"), DESCRIPTION_RIGHT_OF_BACK);
}
