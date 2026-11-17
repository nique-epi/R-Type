#include "AccountFixtures.hpp"
#include <string>
#include "AccountTestConstants.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "UserRole.hpp"

namespace rtype::client::testing {

network::User aceAccount() {
  return network::User{.id = ACE_ID,
                       .nickname = std::string{ACE_ACCOUNT_NICKNAME},
                       .role = network::UserRole::Player};
}

network::OwnedLooks crownOnlyLooks() {
  return network::OwnedLooks{.items = {std::string{CROWN_NAME}}};
}

}  // namespace rtype::client::testing
