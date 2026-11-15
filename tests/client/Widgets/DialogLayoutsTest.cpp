#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "DialogLayout.hpp"
#include "DialogLayouts.hpp"
#include "DialogLayoutsTestConstants.hpp"
#include "DialogText.hpp"

using rtype::client::DialogLayout;
using rtype::client::dialogLayoutOf;
using rtype::client::DialogText;
using rtype::client::testing::DRAWN_DIALOG;
using rtype::client::testing::LARGEST_TEXT;
using rtype::client::testing::NORMAL_TEXT;
using rtype::client::testing::QUIT_HEADING;
using rtype::client::testing::QUIT_MESSAGE;
using rtype::client::testing::QUIT_MESSAGE_FIRST_LINE;
using rtype::client::testing::QUIT_MESSAGE_SECOND_LINE;
using rtype::client::testing::WIDEST_DIALOG;

/**
 * Given the quit dialog, its question and NO and YES
 * When it is laid out at the size the fonts are drawn at
 * Then it keeps the 272 x 96 of its picture and its question on one line
 */
TEST(DialogLayouts, AtTheNormalSizeADialogKeepsItsPicture) {
  const DialogLayout layout =
      dialogLayoutOf(DialogText{.heading = QUIT_HEADING,
                                .message = QUIT_MESSAGE,
                                .rowCount = 0,
                                .barLabels = {"NO", "YES"}},
                     NORMAL_TEXT);

  EXPECT_EQ(layout.size, DRAWN_DIALOG);
  EXPECT_EQ(layout.messageLines,
            std::vector<std::string>{std::string(QUIT_MESSAGE)});
}

/**
 * Given the same dialog
 * When it is laid out at the largest size
 * Then it grows to 432 wide and its question takes two lines, words whole
 */
TEST(DialogLayouts, AtTheLargestSizeTheQuestionWraps) {
  const DialogLayout layout =
      dialogLayoutOf(DialogText{.heading = QUIT_HEADING,
                                .message = QUIT_MESSAGE,
                                .rowCount = 0,
                                .barLabels = {"NO", "YES"}},
                     LARGEST_TEXT);

  EXPECT_EQ(layout.size.x, WIDEST_DIALOG);
  EXPECT_EQ(layout.messageLines,
            (std::vector<std::string>{std::string(QUIT_MESSAGE_FIRST_LINE),
                                      std::string(QUIT_MESSAGE_SECOND_LINE)}));
}
