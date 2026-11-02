#include "ServerSessionLogFixture.hpp"
#include "DebugJournal.hpp"

namespace rtype::client::testing {

const DebugJournal& ServerSessionLogFixture::journal() const {
  return journal_;
}

}  // namespace rtype::client::testing
