#include <gtest/gtest.h>
#include <optional>
#include "MachineCounters.hpp"
#include "ThisMachine.hpp"

using rtype::server::countersOfThisMachine;
using rtype::server::MachineCounters;

/**
 * Given a machine of a system the server runs on
 * When its counters are read
 * Then the system gives them: a machine that holds memory, of which no more
 * than all is in use, and processors that were idle for no longer than they
 * ran
 */
TEST(ThisMachine, IsCountedOnTheSystemsTheServerRunsOn) {
  const std::optional<MachineCounters> read = countersOfThisMachine();

  ASSERT_TRUE(read.has_value());
  const MachineCounters counters = read.value_or(MachineCounters{});
  EXPECT_GT(counters.totalMemoryBytes, 0U);
  EXPECT_LE(counters.usedMemoryBytes, counters.totalMemoryBytes);
  EXPECT_GT(counters.processorTime, 0U);
  EXPECT_LE(counters.idleProcessorTime, counters.processorTime);
}
