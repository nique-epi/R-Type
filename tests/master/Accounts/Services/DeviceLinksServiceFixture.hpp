#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <variant>
#include "AccountsService.hpp"
#include "CreatedDeviceLink.hpp"
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinksService.hpp"
#include "MasterDatabaseFile.hpp"
#include "User.hpp"

/** @brief The device links of the master on a database of their own, with
 * the account ACE, the pages on TEST_SITE_URL. */
class DeviceLinksServiceFixture : public ::testing::Test {
 protected:
  DeviceLinksServiceFixture();

  /** @returns The links the tests ask for. */
  [[nodiscard]] rtype::master::DeviceLinksService& links();

  /** @returns A new link for GAME_ADDRESS at LINK_OPENED_AT; a link with no
   * secret when it was refused. */
  [[nodiscard]] rtype::network::CreatedDeviceLink openLink();

  /** @returns What ACE deciding @p decision on @p link at @p date answers. */
  std::variant<rtype::network::DeviceLinkState,
               rtype::master::DeviceLinkRefusal>
  decideAsAce(const rtype::network::CreatedDeviceLink& link,
              rtype::network::DeviceLinkDecision decision,
              std::chrono::sys_seconds date);

 private:
  MasterDatabaseFile file_;
  rtype::master::AccountsService accounts_;
  rtype::master::DeviceLinksService links_;
  rtype::network::User ace_;
};
