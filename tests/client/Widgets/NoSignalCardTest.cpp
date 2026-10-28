#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "NoSignalCardFixtures.hpp"
#include "NoSignalCardTestConstants.hpp"
#include "NoSignalLook.hpp"
#include "NoSignalReason.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::NoSignalLook;
using rtype::client::NoSignalReason;
using testing::AllOf;
using testing::Contains;
using testing::Field;

/**
 * Given the card of a silent server
 * When it is painted
 * Then the 75 % gray bar fills the left of the two top thirds
 */
TEST(NoSignalCard, PaintsTheGrayBarFirst) {
  const InterfaceCanvasRecord record = cardPaintingOf(NoSignalLook{});

  EXPECT_THAT(record.fills,
              Contains(AllOf(Field(&RecordedFill::area, GRAY_BAR),
                             Field(&RecordedFill::color, SEVENTY_FIVE_GRAY))));
}

/**
 * Given the card of a silent server
 * When it is painted
 * Then the white at 100 % lies in the bottom row, second from the left
 */
TEST(NoSignalCard, PaintsTheWhiteOfTheBottomRow) {
  const InterfaceCanvasRecord record = cardPaintingOf(NoSignalLook{});

  EXPECT_THAT(record.fills,
              Contains(AllOf(Field(&RecordedFill::area, BOTTOM_WHITE),
                             Field(&RecordedFill::color, FULL_WHITE))));
}

/**
 * Given the card of a silent server
 * When it is painted
 * Then its box says NO SIGNAL and that the server stopped answering
 */
TEST(NoSignalCard, SaysASilentServerStoppedAnswering) {
  const InterfaceCanvasRecord record =
      cardPaintingOf(NoSignalLook{.reason = NoSignalReason::SilentServer});

  EXPECT_NE(writingOf(record, NO_SIGNAL_WORDS), nullptr);
  EXPECT_NE(writingOf(record, SILENT_WORDS), nullptr);
}

/**
 * Given the card of a server that closed the session
 * When it is painted
 * Then its box says that the server shut down
 */
TEST(NoSignalCard, SaysAClosedServerShutDown) {
  const InterfaceCanvasRecord record =
      cardPaintingOf(NoSignalLook{.reason = NoSignalReason::ClosedServer});

  EXPECT_NE(writingOf(record, CLOSED_WORDS), nullptr);
}

/**
 * Given a card shown long enough for the call to be typed
 * When it is painted
 * Then the whole call to reconnect is written
 */
TEST(NoSignalCard, WritesTheWholeCallOnceTyped) {
  const InterfaceCanvasRecord record =
      cardPaintingOf(NoSignalLook{.sinceCard = CALL_TYPED});

  EXPECT_NE(writingOf(record, RECONNECT_WORDS), nullptr);
}

/**
 * Given a card that has just shown
 * When it is painted
 * Then the call is not written yet: it waits for its delay
 */
TEST(NoSignalCard, WritesNoCallBeforeItsDelay) {
  const InterfaceCanvasRecord record = cardPaintingOf(NoSignalLook{});

  EXPECT_EQ(writingOf(record, RECONNECT_WORDS), nullptr);
}
