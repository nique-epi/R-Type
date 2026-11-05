#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "OrderedReceiver.hpp"
#include "ReliabilityFixtures.hpp"
#include "ReliabilityTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a receiver that expects number 1
 * When numbers 3, 1 and 2 arrive in that order
 * Then nothing is handed over for 3, then 1, then 2 and 3 together
 */
TEST(OrderedReceiver, HandsOverMessagesInTheOrderTheyWereNumbered) {
  OrderedReceiver receiver;

  const auto afterThird =
      carriedNumbers(receiver.receive(envelopeOf(THIRD_NUMBER, FIRST_NUMBER)));
  const auto afterFirst =
      carriedNumbers(receiver.receive(envelopeOf(FIRST_NUMBER, FIRST_NUMBER)));
  const auto afterSecond =
      carriedNumbers(receiver.receive(envelopeOf(SECOND_NUMBER, FIRST_NUMBER)));

  EXPECT_TRUE(afterThird.empty());
  EXPECT_EQ(afterFirst, (std::vector<std::uint16_t>{FIRST_NUMBER}));
  EXPECT_EQ(afterSecond,
            (std::vector<std::uint16_t>{SECOND_NUMBER, THIRD_NUMBER}));
}

/**
 * Given a receiver that expects number 1
 * When number 2 arrives
 * Then nothing is handed over and one message is held
 */
TEST(OrderedReceiver, HoldsAMessageUntilTheOnesBeforeItArrive) {
  OrderedReceiver receiver;

  const auto handedOver =
      receiver.receive(envelopeOf(SECOND_NUMBER, FIRST_NUMBER));

  EXPECT_TRUE(handedOver.empty());
  EXPECT_EQ(receiver.heldCount(), 1U);
}

/**
 * Given a receiver that handed number 1 over
 * When number 1 arrives again
 * Then nothing is handed over
 */
TEST(OrderedReceiver, HandsOverADuplicateOnlyOnce) {
  OrderedReceiver receiver;
  ASSERT_EQ(receiver.receive(envelopeOf(FIRST_NUMBER, FIRST_NUMBER)).size(),
            1U);

  const auto again = receiver.receive(envelopeOf(FIRST_NUMBER, FIRST_NUMBER));

  EXPECT_TRUE(again.empty());
  EXPECT_EQ(receiver.heldCount(), 0U);
}

/**
 * Given a receiver that holds number 2 while 1 is missing
 * When number 2 arrives again
 * Then it is held once, and 1 and 2 are handed over when 1 arrives
 */
TEST(OrderedReceiver, HoldsADuplicateOfAHeldMessageOnce) {
  OrderedReceiver receiver;
  ASSERT_TRUE(
      receiver.receive(envelopeOf(SECOND_NUMBER, FIRST_NUMBER)).empty());

  const auto again = receiver.receive(envelopeOf(SECOND_NUMBER, FIRST_NUMBER));
  const auto afterFirst =
      carriedNumbers(receiver.receive(envelopeOf(FIRST_NUMBER, FIRST_NUMBER)));

  EXPECT_TRUE(again.empty());
  EXPECT_EQ(afterFirst,
            (std::vector<std::uint16_t>{FIRST_NUMBER, SECOND_NUMBER}));
}

/**
 * Given a receiver that expects number 1
 * When number 257, 256 numbers ahead, arrives
 * Then it is ignored
 */
TEST(OrderedReceiver, IgnoresTheFirstNumberBeyondTheWindow) {
  OrderedReceiver receiver;

  const auto handedOver = receiver.receive(
      envelopeOf(FIRST_NUMBER_BEYOND_THE_WINDOW, FIRST_NUMBER));

  EXPECT_TRUE(handedOver.empty());
  EXPECT_EQ(receiver.heldCount(), 0U);
}

/**
 * Given a receiver that expects number 1
 * When number 256, the last of the window, arrives
 * Then it is held
 */
TEST(OrderedReceiver, HoldsTheLastNumberOfTheWindow) {
  OrderedReceiver receiver;

  const auto handedOver =
      receiver.receive(envelopeOf(LAST_NUMBER_OF_THE_WINDOW, FIRST_NUMBER));

  EXPECT_TRUE(handedOver.empty());
  EXPECT_EQ(receiver.heldCount(), 1U);
}

/**
 * Given a receiver that expects number 1
 * When number 4 arrives, its sender keeping nothing before 4
 * Then 4 is handed over: the sender gave up 1 to 3
 */
TEST(OrderedReceiver, SkipsAGapTheSenderGaveUp) {
  OrderedReceiver receiver;

  const auto handedOver = carriedNumbers(
      receiver.receive(envelopeOf(FOURTH_NUMBER, FOURTH_NUMBER)));

  EXPECT_EQ(handedOver, (std::vector<std::uint16_t>{FOURTH_NUMBER}));
}

/**
 * Given a receiver that holds number 2 while 1 is missing
 * When number 5 arrives, its sender keeping nothing before 5
 * Then 2 is handed over before 5: it was received, only 1, 3 and 4 were given
 * up
 */
TEST(OrderedReceiver, HandsOverTheMessagesHeldBeforeTheSkip) {
  OrderedReceiver receiver;
  ASSERT_TRUE(
      receiver.receive(envelopeOf(SECOND_NUMBER, FIRST_NUMBER)).empty());

  const auto handedOver =
      carriedNumbers(receiver.receive(envelopeOf(FIFTH_NUMBER, FIFTH_NUMBER)));

  EXPECT_EQ(handedOver,
            (std::vector<std::uint16_t>{SECOND_NUMBER, FIFTH_NUMBER}));
}

/**
 * Given a receiver that skipped, step by step, to number 65534
 * When 65535 and 0 arrive in the reverse order
 * Then nothing is handed over for 0, then 65535 and 0 together
 */
TEST(OrderedReceiver, WorksAcrossTheWrapOfTheNumbers) {
  OrderedReceiver receiver;
  ASSERT_EQ(receiver.receive(envelopeOf(MIDDLE_NUMBER, MIDDLE_NUMBER)).size(),
            1U);
  ASSERT_EQ(receiver.receive(envelopeOf(HIGH_NUMBER, HIGH_NUMBER)).size(), 1U);
  ASSERT_EQ(
      receiver
          .receive(envelopeOf(NUMBER_BEFORE_THE_WRAP, NUMBER_BEFORE_THE_WRAP))
          .size(),
      1U);

  const auto afterZero = receiver.receive(
      envelopeOf(NUMBER_AFTER_THE_WRAP, NUMBER_BEFORE_THE_WRAP));
  const auto afterLast = carriedNumbers(receiver.receive(
      envelopeOf(LAST_NUMBER_BEFORE_THE_WRAP, NUMBER_BEFORE_THE_WRAP)));

  EXPECT_TRUE(afterZero.empty());
  EXPECT_EQ(afterLast, (std::vector<std::uint16_t>{LAST_NUMBER_BEFORE_THE_WRAP,
                                                   NUMBER_AFTER_THE_WRAP}));
}

/**
 * Given a receiver that expects number 1
 * When number 20000 arrives, its sender keeping nothing before it
 * Then only 20000 is handed over, and the receiver expects 20001 next
 */
TEST(OrderedReceiver, SkipsAFarGapWithoutHandingOverAnythingElse) {
  OrderedReceiver receiver;

  const auto handedOver = carriedNumbers(
      receiver.receive(envelopeOf(FAR_AHEAD_NUMBER, FAR_AHEAD_NUMBER)));

  EXPECT_EQ(handedOver, (std::vector<std::uint16_t>{FAR_AHEAD_NUMBER}));
  EXPECT_EQ(receiver.heldCount(), 0U);
}

/**
 * Given a receiver that holds number 2 while 1 is missing
 * When it is reset and number 1 arrives
 * Then only 1 is handed over and nothing is held
 */
TEST(OrderedReceiver, ForgetsEverythingOnReset) {
  OrderedReceiver receiver;
  ASSERT_TRUE(
      receiver.receive(envelopeOf(SECOND_NUMBER, FIRST_NUMBER)).empty());

  receiver.reset();
  const auto handedOver =
      carriedNumbers(receiver.receive(envelopeOf(FIRST_NUMBER, FIRST_NUMBER)));

  EXPECT_EQ(handedOver, (std::vector<std::uint16_t>{FIRST_NUMBER}));
  EXPECT_EQ(receiver.heldCount(), 0U);
}

}  // namespace rtype::network::testing
