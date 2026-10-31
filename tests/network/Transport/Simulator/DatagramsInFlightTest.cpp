#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <vector>
#include "DatagramInFlight.hpp"
#include "DatagramsInFlight.hpp"
#include "DatagramsInFlightFixtures.hpp"
#include "DatagramsInFlightTestConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "SimulatorFixtures.hpp"
#include "SimulatorTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::network::DatagramInFlight;
using rtype::network::DatagramsInFlight;
using rtype::network::NetworkConditions;

/**
 * Given a perfect network
 * When a datagram is handed over
 * Then it arrives at once, to the same peer, with the same bytes
 */
TEST(DatagramsInFlight, HandsADatagramAtOnceOverAPerfectNetwork) {
  DatagramsInFlight inFlight{NetworkConditions{}, SOME_SEED};

  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START);
  const std::vector<DatagramInFlight> arrived =
      inFlight.takeArrived(HAND_OVER_START);

  EXPECT_EQ(arrived, (std::vector<DatagramInFlight>{
                         DatagramInFlight{.arrival = HAND_OVER_START,
                                          .peer = peer(),
                                          .payload = crossingBytes()}}));
}

/**
 * Given a lag of 200 ms
 * When a datagram is handed over
 * Then it has not arrived one nanosecond before 100 ms passed
 */
TEST(DatagramsInFlight, HoldsADatagramUntilHalfTheLagPassed) {
  DatagramsInFlight inFlight{LAGGING_NETWORK, SOME_SEED};

  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START);
  const std::vector<DatagramInFlight> arrived =
      inFlight.takeArrived(HAND_OVER_START + HALF_OF_THE_LAG - ONE_NANOSECOND);

  EXPECT_TRUE(arrived.empty());
}

/**
 * Given a lag of 200 ms
 * When a datagram is handed over
 * Then it arrives once 100 ms passed
 */
TEST(DatagramsInFlight, HandsADatagramOnceHalfTheLagPassed) {
  DatagramsInFlight inFlight{LAGGING_NETWORK, SOME_SEED};

  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START);
  const std::vector<DatagramInFlight> arrived =
      inFlight.takeArrived(HAND_OVER_START + HALF_OF_THE_LAG);

  EXPECT_EQ(arrived.size(), 1U);
}

/**
 * Given a lag of 200 ms
 * When a datagram is handed over
 * Then the next arrival is 100 ms after the hand over
 */
TEST(DatagramsInFlight, GivesHalfTheLagAsTheNextArrival) {
  DatagramsInFlight inFlight{LAGGING_NETWORK, SOME_SEED};

  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START);

  EXPECT_EQ(inFlight.nextArrival(), HAND_OVER_START + HALF_OF_THE_LAG);
}

/**
 * Given a datagram in flight
 * When it arrived and was taken
 * Then nothing is left in flight: there is no next arrival
 */
TEST(DatagramsInFlight, HasNoNextArrivalOnceEverythingArrived) {
  DatagramsInFlight inFlight{LAGGING_NETWORK, SOME_SEED};
  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START);

  static_cast<void>(everyArrival(inFlight));

  EXPECT_FALSE(inFlight.nextArrival().has_value());
}

/**
 * Given a lag of 200 ms and two datagrams handed over 20 ms apart
 * When the first arrived and was taken
 * Then the second is still in flight, due 100 ms after its own hand over
 */
TEST(DatagramsInFlight, KeepsTheDatagramsThatHaveNotArrivedYet) {
  DatagramsInFlight inFlight{LAGGING_NETWORK, SOME_SEED};
  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START);
  inFlight.add(peer(), CROSSING_BYTES, HAND_OVER_START + SECOND_HAND_OVER);

  static_cast<void>(inFlight.takeArrived(HAND_OVER_START + HALF_OF_THE_LAG));

  EXPECT_EQ(inFlight.nextArrival(),
            HAND_OVER_START + SECOND_HAND_OVER + HALF_OF_THE_LAG);
}

/**
 * Given a jitter of 60 ms and 1000 datagrams handed over 1 ms apart
 * When every datagram arrived
 * Then they arrived in the order they were handed over
 */
TEST(DatagramsInFlight, JitterNeverMakesADatagramArriveBeforeAnEarlierOne) {
  DatagramsInFlight inFlight{JITTER_OF_60_MS, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);
  const std::vector<DatagramInFlight> arrivals = everyArrival(inFlight);

  ASSERT_EQ(arrivals.size(), THOUSAND_DATAGRAMS);
  EXPECT_TRUE(std::ranges::is_sorted(arrivals, {}, numberOf));
}

/**
 * Given a jitter of 60 ms and 1000 datagrams handed over 100 ms apart
 * When every datagram arrived
 * Then none waited more than 30 ms
 */
TEST(DatagramsInFlight, JitterDelaysEachDatagramByAtMostHalfOfIt) {
  DatagramsInFlight inFlight{JITTER_OF_60_MS, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, HUNDRED_MILLISECONDS_APART);
  const std::vector<DatagramInFlight> arrivals = everyArrival(inFlight);

  EXPECT_LE(std::ranges::max(waitsOf(arrivals, HUNDRED_MILLISECONDS_APART)),
            HALF_OF_60_MS);
}

/**
 * Given a jitter of 60 ms and 1000 datagrams handed over 100 ms apart
 * When every datagram arrived
 * Then some waited less than 5 ms and some more than 25 ms
 */
TEST(DatagramsInFlight, JitterSpreadsTheWaitsOverHalfOfIt) {
  DatagramsInFlight inFlight{JITTER_OF_60_MS, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, HUNDRED_MILLISECONDS_APART);
  const std::vector<std::chrono::steady_clock::duration> waits =
      waitsOf(everyArrival(inFlight), HUNDRED_MILLISECONDS_APART);

  EXPECT_LT(std::ranges::min(waits), NEAR_NO_WAIT);
  EXPECT_GT(std::ranges::max(waits), NEAR_HALF_OF_60_MS);
}

/**
 * Given a loss of 100 %
 * When 1000 datagrams are handed over
 * Then none arrives
 */
TEST(DatagramsInFlight, LosesEveryDatagramAtAFullLoss) {
  DatagramsInFlight inFlight{FULL_LOSS, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);

  EXPECT_TRUE(everyArrival(inFlight).empty());
}

/**
 * Given a loss of 10 %
 * When 10 000 datagrams are handed over
 * Then about 9000 arrive
 */
TEST(DatagramsInFlight, LosesAboutTheShareOfTheLoss) {
  DatagramsInFlight inFlight{TENTH_LOSS, SOME_SEED};

  handNumbered(inFlight, TEN_THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);

  EXPECT_NEAR(static_cast<double>(everyArrival(inFlight).size()),
              ARRIVALS_AT_A_TENTH_LOSS, DRAW_TOLERANCE);
}

/**
 * Given duplicates at 100 %
 * When 1000 datagrams are handed over
 * Then each arrives twice
 */
TEST(DatagramsInFlight, CopiesEveryDatagramAtFullDuplicates) {
  DatagramsInFlight inFlight{FULL_DUPLICATES, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);
  const std::vector<std::size_t> counts =
      arrivalsByNumber(everyArrival(inFlight), THOUSAND_DATAGRAMS);

  EXPECT_TRUE(std::ranges::all_of(counts, [](std::size_t count) {
    return count == ARRIVALS_OF_A_COPIED_DATAGRAM;
  }));
}

/**
 * Given duplicates at 10 %
 * When 10 000 datagrams are handed over
 * Then about 11 000 arrive
 */
TEST(DatagramsInFlight, CopiesAboutTheShareOfTheDuplicates) {
  DatagramsInFlight inFlight{TENTH_DUPLICATES, SOME_SEED};

  handNumbered(inFlight, TEN_THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);

  EXPECT_NEAR(static_cast<double>(everyArrival(inFlight).size()),
              ARRIVALS_AT_A_TENTH_DUPLICATES, DRAW_TOLERANCE);
}

/**
 * Given a loss of 50 % and duplicates at 100 %
 * When 1000 datagrams are handed over
 * Then each arrives twice or never, never once: a lost datagram is not copied
 */
TEST(DatagramsInFlight, NeverCopiesALostDatagram) {
  DatagramsInFlight inFlight{HALF_LOSS_FULL_DUPLICATES, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);
  const std::vector<std::size_t> counts =
      arrivalsByNumber(everyArrival(inFlight), THOUSAND_DATAGRAMS);

  EXPECT_TRUE(std::ranges::none_of(
      counts, [](std::size_t count) { return count == 1; }));
}

/**
 * Given reordering at 100 % and no lag
 * When 1000 datagrams are handed over 100 ms apart
 * Then each waits exactly 75 ms
 */
TEST(DatagramsInFlight, HoldsAReorderedDatagramForTheReorderHold) {
  DatagramsInFlight inFlight{FULL_REORDER, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, HUNDRED_MILLISECONDS_APART);

  EXPECT_TRUE(std::ranges::all_of(
      waitsOf(everyArrival(inFlight), HUNDRED_MILLISECONDS_APART),
      [](std::chrono::steady_clock::duration wait) {
        return wait == HOLD_OF_A_REORDERED_DATAGRAM;
      }));
}

/**
 * Given reordering at 50 % and 1000 datagrams handed over 10 ms apart
 * When every datagram arrived
 * Then some arrived before a datagram handed over before them
 */
TEST(DatagramsInFlight, LetsTheNextDatagramsArriveBeforeAHeldOne) {
  DatagramsInFlight inFlight{HALF_REORDER, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, TEN_MILLISECONDS_APART);

  EXPECT_FALSE(std::ranges::is_sorted(everyArrival(inFlight), {}, numberOf));
}

/**
 * Given reordering at 50 % and no lag
 * When 1000 datagrams are handed over 100 ms apart
 * Then each waits either nothing or 75 ms, and both happen
 */
TEST(DatagramsInFlight, HoldsOnlyTheDrawnDatagrams) {
  DatagramsInFlight inFlight{HALF_REORDER, SOME_SEED};

  handNumbered(inFlight, THOUSAND_DATAGRAMS, HUNDRED_MILLISECONDS_APART);
  const std::vector<std::chrono::steady_clock::duration> waits =
      waitsOf(everyArrival(inFlight), HUNDRED_MILLISECONDS_APART);
  const auto held = std::ranges::count(waits, HOLD_OF_A_REORDERED_DATAGRAM);
  const auto onTime =
      std::ranges::count(waits, std::chrono::steady_clock::duration::zero());

  EXPECT_EQ(static_cast<std::size_t>(held + onTime), THOUSAND_DATAGRAMS);
  EXPECT_GT(held, 0);
  EXPECT_GT(onTime, 0);
}

/**
 * Given two simulated networks with every defect and the same seed
 * When both are handed the same datagrams at the same times
 * Then they deliver the same arrivals
 */
TEST(DatagramsInFlight, DrawsTheSameFateFromTheSameSeed) {
  DatagramsInFlight first{EVERY_DEFECT, SOME_SEED};
  DatagramsInFlight second{EVERY_DEFECT, SOME_SEED};

  handNumbered(first, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);
  handNumbered(second, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);

  EXPECT_EQ(everyArrival(first), everyArrival(second));
}

/**
 * Given two simulated networks with every defect and two seeds
 * When both are handed the same datagrams at the same times
 * Then their arrivals differ
 */
TEST(DatagramsInFlight, DrawsAnotherFateFromAnotherSeed) {
  DatagramsInFlight first{EVERY_DEFECT, SOME_SEED};
  DatagramsInFlight second{EVERY_DEFECT, OTHER_SEED};

  handNumbered(first, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);
  handNumbered(second, THOUSAND_DATAGRAMS, ONE_MILLISECOND_APART);

  EXPECT_NE(everyArrival(first), everyArrival(second));
}
