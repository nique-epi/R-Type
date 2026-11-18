#include "MasterAccountLinkFixtures.hpp"
#include <string>
#include "MasterAccountLinkTestConstants.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "UserRole.hpp"

namespace rtype::client::testing {

network::User linkAce() {
  return network::User{.id = LINK_ACE_ID,
                       .nickname = std::string{LINK_ACE_NICKNAME},
                       .role = network::UserRole::Player};
}

network::OwnedLooks linkLooks() {
  return network::OwnedLooks{.items = {"top_hat", "cool_shades", "mustache"}};
}

}  // namespace rtype::client::testing
