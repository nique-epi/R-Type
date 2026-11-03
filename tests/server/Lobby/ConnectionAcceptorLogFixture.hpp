#pragma once

#include <string>
#include <vector>
#include "ConnectionAcceptorFixture.hpp"
#include "DebugJournal.hpp"

namespace rtype::server::testing {

/** @brief The acceptor, with the log written to a journal the test reads. */
class ConnectionAcceptorLogFixture : public ConnectionAcceptorFixture {
 protected:
  /** @returns The lines of the journal about a datagram dropped. */
  [[nodiscard]] std::vector<std::string> dropLines() const;

 private:
  DebugJournal journal_;
};

}  // namespace rtype::server::testing
