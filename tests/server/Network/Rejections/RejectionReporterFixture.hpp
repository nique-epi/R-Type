#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "DebugJournal.hpp"
#include "RejectionLedger.hpp"
#include "RejectionReporter.hpp"

/**
 * @brief A reporter on a ledger that counted one stranger, with the log written
 * to a journal the test reads.
 */
class RejectionReporterFixture : public ::testing::Test {
 protected:
  RejectionReporterFixture();

  /** @brief Counts @p count ticks on the reporter. */
  void countTicks(std::size_t count);

  /** @brief Counts one more stranger in the ledger. */
  void rejectOneStranger();

  /** @returns How many lines of the rejection report the journal holds. */
  [[nodiscard]] std::size_t reportLineCount() const;

 private:
  DebugJournal journal_;
  rtype::server::RejectionLedger ledger_;
  rtype::server::RejectionReporter reporter_;
};
