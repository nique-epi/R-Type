#include "SignInFixtures.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "SignInTestConstants.hpp"

namespace rtype::client::testing {

DeviceLinkReading waitingReading(std::uint64_t ask,
                                 std::chrono::sys_seconds expiresAt) {
  return DeviceLinkReading{
      .ask = ask,
      .phase = DeviceLinkPhase::Waiting,
      .userCode = std::string{SHOWN_CODE},
      .verificationUri = std::string{SHOWN_PAGE},
      .verificationUriComplete = std::string{SHOWN_PAGE_WITH_CODE},
      .expiresAt = expiresAt,
      .token = {}};
}

DeviceLinkReading readingAt(std::uint64_t ask, DeviceLinkPhase phase) {
  DeviceLinkReading reading =
      waitingReading(ask, NINE_AND_A_HALF_MINUTES_LATER);
  reading.phase = phase;
  if (phase == DeviceLinkPhase::SignedIn) {
    reading.token = std::string{SIGN_IN_TOKEN};
  }
  return reading;
}

}  // namespace rtype::client::testing
