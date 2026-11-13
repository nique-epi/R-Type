#include <gtest/gtest.h>
#include <optional>
#include "MachineCounters.hpp"
#include "ProcCounters.hpp"
#include "ProcCountersTestConstants.hpp"
#include "ProcTexts.hpp"

using rtype::server::countersIn;
using rtype::server::MachineCounters;
using rtype::server::ProcTexts;

/**
 * Given the files of a Linux machine of two processors and 8 GiB, 5 GiB of
 * them available
 * When its counters are read
 * Then the processor time adds the eight times of the line of every
 * processor, without the guest times, its idle part adds the idle time and
 * the wait for the disk, and 3 GiB of 8 are in use
 */
TEST(ProcCounters, ReadsTheTimesOfEveryProcessorAndTheMemoryInUse) {
  const std::optional<MachineCounters> counters = countersIn(ProcTexts{
      .processors = TWO_PROCESSORS_TEXT, .memory = EIGHT_GIBIBYTES_TEXT});

  ASSERT_TRUE(counters.has_value());
  EXPECT_EQ(counters.value_or(MachineCounters{}),
            (MachineCounters{.processorTime = TWO_PROCESSORS_TIME,
                             .idleProcessorTime = TWO_PROCESSORS_IDLE_TIME,
                             .usedMemoryBytes = THREE_GIBIBYTES,
                             .totalMemoryBytes = EIGHT_GIBIBYTES}));
}

/**
 * Given the line of an old kernel, which writes four times only
 * When the counters are read
 * Then the processor time adds the four, and its idle part is the idle time
 */
TEST(ProcCounters, ReadsALineOfFourTimes) {
  const MachineCounters counters =
      countersIn(ProcTexts{.processors = FOUR_FIELDS_PROCESSORS_TEXT,
                           .memory = EIGHT_GIBIBYTES_TEXT})
          .value_or(MachineCounters{});

  EXPECT_EQ(counters.processorTime, FOUR_FIELDS_TIME);
  EXPECT_EQ(counters.idleProcessorTime, FOUR_FIELDS_IDLE_TIME);
}

/**
 * Given a text with the lines of each processor but not the line that adds
 * them up
 * When the counters are read
 * Then there are none
 */
TEST(ProcCounters, GivesNothingWithoutTheLineOfEveryProcessor) {
  EXPECT_FALSE(countersIn(ProcTexts{.processors = NO_SUM_PROCESSORS_TEXT,
                                    .memory = EIGHT_GIBIBYTES_TEXT})
                   .has_value());
}

/**
 * Given a line of three times, one fewer than any kernel writes
 * When the counters are read
 * Then there are none
 */
TEST(ProcCounters, GivesNothingForALineOfThreeTimes) {
  EXPECT_FALSE(countersIn(ProcTexts{.processors = THREE_FIELDS_PROCESSORS_TEXT,
                                    .memory = EIGHT_GIBIBYTES_TEXT})
                   .has_value());
}

/**
 * Given a memory text without the available memory
 * When the counters are read
 * Then there are none, since the free memory alone leaves out the cache
 */
TEST(ProcCounters, GivesNothingWithoutTheAvailableMemory) {
  EXPECT_FALSE(countersIn(ProcTexts{.processors = TWO_PROCESSORS_TEXT,
                                    .memory = NO_AVAILABLE_MEMORY_TEXT})
                   .has_value());
}

/**
 * Given a memory text with one kibibyte more available than the machine holds
 * When the counters are read
 * Then there are none
 */
TEST(ProcCounters, GivesNothingWhenMoreIsAvailableThanTheMachineHolds) {
  EXPECT_FALSE(countersIn(ProcTexts{.processors = TWO_PROCESSORS_TEXT,
                                    .memory = MORE_AVAILABLE_THAN_TOTAL_TEXT})
                   .has_value());
}

/**
 * Given a memory text whose total is a word
 * When the counters are read
 * Then there are none
 */
TEST(ProcCounters, GivesNothingForATotalThatIsNoNumber) {
  EXPECT_FALSE(countersIn(ProcTexts{.processors = TWO_PROCESSORS_TEXT,
                                    .memory = WORDY_TOTAL_MEMORY_TEXT})
                   .has_value());
}
