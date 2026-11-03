#pragma once

#include <string>
#include <vector>
#include "DatagramSorterFixture.hpp"
#include "DebugJournal.hpp"

/** @brief The sorter, with the log written to a journal the test reads. */
class DatagramSorterLogFixture : public DatagramSorterFixture {
 protected:
  /** @returns The lines of the journal about a datagram dropped. */
  [[nodiscard]] std::vector<std::string> dropLines() const;

 private:
  DebugJournal journal_;
};
