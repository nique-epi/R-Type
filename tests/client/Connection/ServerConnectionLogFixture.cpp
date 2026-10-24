#include "ServerConnectionLogFixture.hpp"
#include "DebugJournal.hpp"

namespace rtype::client::testing {

const DebugJournal& ServerConnectionLogFixture::journal() const {
  return journal_;
}

}  // namespace rtype::client::testing
