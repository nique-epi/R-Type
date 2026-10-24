#pragma once

#include "DebugJournal.hpp"
#include "ServerConnectionFixture.hpp"

namespace rtype::client::testing {

/** @brief The connection, with the log written to a journal the test reads. */
class ServerConnectionLogFixture : public ServerConnectionFixture {
 protected:
  [[nodiscard]] const DebugJournal& journal() const;

 private:
  DebugJournal journal_;
};

}  // namespace rtype::client::testing
