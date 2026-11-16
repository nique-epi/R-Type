#include "AccountSessionFixture.hpp"
#include <string>
#include "AccountSession.hpp"
#include "AccountTestConstants.hpp"
#include "RecordingAccountLink.hpp"
#include "UserReading.hpp"

namespace rtype::client::testing {

AccountSessionFixture::AccountSessionFixture() : session_(link_) {}

RecordingAccountLink& AccountSessionFixture::link() { return link_; }

AccountSession& AccountSessionFixture::session() { return session_; }

void AccountSessionFixture::signInAsAce() {
  session_.signIn(std::string{ACE_TOKEN});
  link_.userRead =
      UserReading{.ask = FIRST_ASK, .user = ACE_ACCOUNT, .refusal = {}};
}

}  // namespace rtype::client::testing
