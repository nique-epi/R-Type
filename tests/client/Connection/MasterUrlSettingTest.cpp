#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "MasterUrlSetting.hpp"
#include "MasterUrlSettingTestConstants.hpp"

namespace rtype::client {

using testing::MASTER_ON_THIS_MACHINE;
using testing::REMOTE_MASTER_URL;
using testing::SECURE_MASTER_URL;

/**
 * Given no setting that names a master
 * When the client asks for the master it reads by default
 * Then it is the master on this machine, on the default port of a master
 */
TEST(MasterUrlSetting, TheDefaultIsTheMasterOnThisMachine) {
  EXPECT_EQ(defaultMasterUrl(), MASTER_ON_THIS_MACHINE);
}

/**
 * Given the url of a master on another machine
 * When the setting checks it
 * Then it is kept
 */
TEST(MasterUrlSetting, AcceptsTheUrlOfARemoteMaster) {
  EXPECT_NO_THROW(requireMasterUrl(REMOTE_MASTER_URL));
}

/**
 * Given an https url, which the client cannot call
 * When the setting checks it
 * Then it is refused, so the master it had stays
 */
TEST(MasterUrlSetting, RefusesAnHttpsUrl) {
  EXPECT_THROW(requireMasterUrl(SECURE_MASTER_URL),
               engine::CommandRefusedException);
}

}  // namespace rtype::client
