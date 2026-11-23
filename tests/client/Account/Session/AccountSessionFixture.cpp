#include "AccountSessionFixture.hpp"
#include <string>
#include "AccountFixtures.hpp"
#include "AccountSession.hpp"
#include "AccountTestConstants.hpp"
#include "RecordingAccountLink.hpp"
#include "RecordingSessionTokenStorage.hpp"
#include "UserReading.hpp"

namespace rtype::client::testing {

AccountSessionFixture::AccountSessionFixture() : session_(link_, storage_) {}

RecordingAccountLink& AccountSessionFixture::link() { return link_; }

AccountSession& AccountSessionFixture::session() { return session_; }

RecordingSessionTokenStorage& AccountSessionFixture::storage() {
  return storage_;
}

void AccountSessionFixture::signInAsAce() {
  session_.signIn(std::string{ACE_TOKEN});
  link_.userRead =
      UserReading{.ask = FIRST_ASK, .user = aceAccount(), .refusal = {}};
}

}  // namespace rtype::client::testing
