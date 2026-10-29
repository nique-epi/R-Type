#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ArrivalLedger.hpp"
#include "ArrivalLedgerConstants.hpp"
#include "ArrivalLedgerTestConstants.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::FIRST_SEQUENCE;
using testing::HALF_RANGE_DISTANCE;
using testing::JUST_UNDER_HALF_RANGE_DISTANCE;
using testing::LAST_SEQUENCE;
using testing::LOSS_PERIOD;
using testing::LOST_OFFSET;
using testing::NEWER_SEQUENCE;
using testing::OLDER_SEQUENCE;
using testing::ONE_IN_TEN;
using testing::SOME_SEQUENCE;

/**
 * Given a ledger that accepted nothing
 * When a datagram arrives
 * Then it is accepted, whatever its sequence
 */
TEST(ArrivalLedger, AcceptsTheFirstDatagram) {
  ArrivalLedger ledger;

  EXPECT_TRUE(ledger.accept(SOME_SEQUENCE));
}

/**
 * Given a ledger that accepted the datagram of sequence 40
 * When a second datagram of sequence 40 arrives
 * Then it is refused
 */
TEST(ArrivalLedger, RefusesADuplicate) {
  ArrivalLedger ledger;
  ASSERT_TRUE(ledger.accept(SOME_SEQUENCE));

  EXPECT_FALSE(ledger.accept(SOME_SEQUENCE));
}

/**
 * Given a ledger that accepted the datagram of sequence 40
 * When the datagram of sequence 39 arrives
 * Then it is refused, and the datagram of sequence 41 is still accepted
 */
TEST(ArrivalLedger, RefusesAnOlderDatagramAndKeepsTheNewestOne) {
  ArrivalLedger ledger;
  ASSERT_TRUE(ledger.accept(SOME_SEQUENCE));

  EXPECT_FALSE(ledger.accept(OLDER_SEQUENCE));
  EXPECT_TRUE(ledger.accept(NEWER_SEQUENCE));
}

/**
 * Given a ledger that accepted the datagram of sequence 65535
 * When the datagram of sequence 0 arrives
 * Then it is accepted, since sequence 0 follows sequence 65535
 */
TEST(ArrivalLedger, AcceptsADatagramOnceTheSequenceWrapsAround) {
  ArrivalLedger ledger;
  ASSERT_TRUE(ledger.accept(LAST_SEQUENCE));

  EXPECT_TRUE(ledger.accept(FIRST_SEQUENCE));
}

/**
 * Given a ledger that accepted the datagram of sequence 0
 * When a datagram ahead by 32767 arrives, then one ahead of that by 32768
 * Then the first is accepted and the second is refused, since exactly half the
 * range is never newer
 */
TEST(ArrivalLedger, TreatsExactlyHalfTheRangeAsNotNewer) {
  ArrivalLedger ledger;
  ASSERT_TRUE(ledger.accept(FIRST_SEQUENCE));
  ASSERT_TRUE(ledger.accept(JUST_UNDER_HALF_RANGE_DISTANCE));

  EXPECT_FALSE(ledger.accept(static_cast<std::uint16_t>(
      JUST_UNDER_HALF_RANGE_DISTANCE + HALF_RANGE_DISTANCE)));
}

/**
 * Given a ledger that accepted a few datagrams
 * When it is asked for its loss rate
 * Then it has none yet, since no window is complete
 */
TEST(ArrivalLedger, HasNoLossRateBeforeTheFirstWindowIsComplete) {
  ArrivalLedger ledger;
  ASSERT_TRUE(ledger.accept(FIRST_SEQUENCE));
  ASSERT_TRUE(ledger.accept(NEWER_SEQUENCE));

  EXPECT_FALSE(ledger.lossRate().has_value());
}

/**
 * Given a ledger and a window of consecutive sequences
 * When the window is complete
 * Then the loss rate is 0
 */
TEST(ArrivalLedger, ReportsNoLossWhenEveryDatagramArrives) {
  ArrivalLedger ledger;

  for (std::size_t sequence = 0; sequence < LOSS_WINDOW_SIZE; ++sequence) {
    ASSERT_TRUE(ledger.accept(static_cast<std::uint16_t>(sequence)));
  }

  EXPECT_EQ(ledger.lossRate(), std::optional<double>{0.0});
}

/**
 * Given a ledger and a window of sequences 0 to 99 of which 5, 15, ... 95 never
 * arrive
 * When the window is complete
 * Then the loss rate is one in ten
 */
TEST(ArrivalLedger, ReportsTheShareOfDatagramsThatNeverArrived) {
  ArrivalLedger ledger;

  for (std::size_t sequence = 0; sequence < LOSS_WINDOW_SIZE; ++sequence) {
    if (sequence % LOSS_PERIOD != LOST_OFFSET) {
      ASSERT_TRUE(ledger.accept(static_cast<std::uint16_t>(sequence)));
    }
  }

  EXPECT_EQ(ledger.lossRate(), std::optional<double>{ONE_IN_TEN});
}

/**
 * Given a ledger that closed a window with one datagram in ten lost
 * When the next window is complete with no loss
 * Then the loss rate is 0: only the last window counts
 */
TEST(ArrivalLedger, MeasuresTheLastCompleteWindowOnly) {
  ArrivalLedger ledger;
  for (std::size_t sequence = 0; sequence < LOSS_WINDOW_SIZE; ++sequence) {
    if (sequence % LOSS_PERIOD != LOST_OFFSET) {
      ASSERT_TRUE(ledger.accept(static_cast<std::uint16_t>(sequence)));
    }
  }

  for (std::size_t sequence = LOSS_WINDOW_SIZE; sequence < 2 * LOSS_WINDOW_SIZE;
       ++sequence) {
    ASSERT_TRUE(ledger.accept(static_cast<std::uint16_t>(sequence)));
  }

  EXPECT_EQ(ledger.lossRate(), std::optional<double>{0.0});
}

/**
 * Given a ledger with a window one datagram short of complete
 * When a duplicate of the newest datagram is refused
 * Then the window is still not complete: a refused datagram counts for nothing
 */
TEST(ArrivalLedger, CountsNothingForARefusedDatagram) {
  ArrivalLedger ledger;
  for (std::size_t sequence = 0; sequence + 1 < LOSS_WINDOW_SIZE; ++sequence) {
    ASSERT_TRUE(ledger.accept(static_cast<std::uint16_t>(sequence)));
  }

  EXPECT_FALSE(ledger.accept(static_cast<std::uint16_t>(LOSS_WINDOW_SIZE - 2)));

  EXPECT_FALSE(ledger.lossRate().has_value());
}

}  // namespace rtype::server
