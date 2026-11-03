#pragma once

#include "DebugJournal.hpp"
#include "ServerSessionFixture.hpp"

namespace rtype::client::testing {

/** @brief The connection, with the log written to a journal the test reads. */
class ServerSessionLogFixture : public ServerSessionFixture {
 protected:
  [[nodiscard]] const DebugJournal& journal() const;

 private:
  DebugJournal journal_;
};

}  // namespace rtype::client::testing
