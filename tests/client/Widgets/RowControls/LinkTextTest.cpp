#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include "BarState.hpp"
#include "LinkText.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::BarState;
using rtype::client::LinkText;

namespace {

/** @brief Where the RESET link of DEFAULT SOUND lies on the picture the team
 * chose. */
constexpr sf::IntRect RESET_LINK({388, 126}, {56, 12});

}  // namespace

/**
 * Given the RESET link of DEFAULT SOUND
 * When its row is activated
 * Then its action runs, once
 */
TEST(LinkTextTest, ActivatingItRunsItsAction) {
  int runs = 0;
  LinkText reset("RESET", [&runs] { ++runs; });
  reset.activate();
  EXPECT_EQ(runs, 1);
}

/**
 * Given the RESET link
 * When it is painted
 * Then it reads RESET >
 */
TEST(LinkTextTest, ItShowsItsWordAndAnArrow) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  const LinkText reset("RESET", [] {});
  reset.paint(canvas, RESET_LINK, BarState::Idle);
  EXPECT_NE(writingOf(record, "RESET >"), nullptr);
}
