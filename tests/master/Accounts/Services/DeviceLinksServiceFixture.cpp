#include "DeviceLinksServiceFixture.hpp"
#include <chrono>
#include <string>
#include <variant>
#include "AccountsService.hpp"
#include "AccountsServiceTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "CreatedDeviceLink.hpp"
#include "CredentialsAttempt.hpp"
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkVerdict.hpp"
#include "DeviceLinksService.hpp"
#include "DeviceLinksServiceTestConstants.hpp"
#include "Hashing.hpp"
#include "MasterTestConstants.hpp"
#include "PasswordKey.hpp"
#include "User.hpp"
#include "UserCredentials.hpp"

DeviceLinksServiceFixture::DeviceLinksServiceFixture()
    : accounts_(file_.database(),
                rtype::network::passwordKeyOfHexadecimal(TEST_PASSWORD_KEY)
                    .value_or(rtype::network::PasswordKey{})),
      links_(file_.database(), accounts_,
             [] { return std::string{TEST_SITE_URL}; }) {
  const auto created = accounts_.createUser(rtype::master::CredentialsAttempt{
      .credentials =
          rtype::network::UserCredentials{
              .nickname = std::string{ACE_NICKNAME},
              .password = std::string{ACE_PASSWORD}},
      .address = std::string{SERVICE_CLIENT_ADDRESS},
      .requestedAt = SESSION_OPENED_AT});
  if (const auto* user = std::get_if<rtype::network::User>(&created)) {
    ace_ = *user;
  }
}

rtype::master::DeviceLinksService& DeviceLinksServiceFixture::links() {
  return links_;
}

rtype::network::CreatedDeviceLink DeviceLinksServiceFixture::openLink() {
  const auto opened = links_.create(std::string{GAME_ADDRESS}, LINK_OPENED_AT);
  const auto* link = std::get_if<rtype::network::CreatedDeviceLink>(&opened);
  return link == nullptr ? rtype::network::CreatedDeviceLink{} : *link;
}

std::variant<rtype::network::DeviceLinkState, rtype::master::DeviceLinkRefusal>
DeviceLinksServiceFixture::decideAsAce(
    const rtype::network::CreatedDeviceLink& link,
    rtype::network::DeviceLinkDecision decision,
    std::chrono::sys_seconds date) {
  return links_.decide(
      link.userCode,
      rtype::master::DeviceLinkVerdict{.userId = ace_.id, .decision = decision},
      date);
}
