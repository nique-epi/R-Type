#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "MasterUrlSetting.hpp"
#include "MasterUrlSettingTestConstants.hpp"

namespace rtype::client {

using testing::PORT_ZERO_MASTER_URL;
using testing::PUBLIC_MASTER_URL;
using testing::REMOTE_MASTER_URL;
using testing::SECURE_MASTER_URL;
using testing::UNKNOWN_SCHEME_MASTER_URL;

/**
 * Given no setting that names a master
 * When the client asks for the master it reads by default
 * Then it is the public master of the game, over HTTPS
 */
TEST(MasterUrlSetting, TheDefaultIsThePublicMaster) {
  EXPECT_EQ(defaultMasterUrl(), PUBLIC_MASTER_URL);
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
 * Given the https url of a master behind TLS
 * When the setting checks it
 * Then it is kept
 */
TEST(MasterUrlSetting, AcceptsAnHttpsUrl) {
  EXPECT_NO_THROW(requireMasterUrl(SECURE_MASTER_URL));
}

/**
 * Given a url whose scheme is neither http nor https
 * When the setting checks it
 * Then it is refused, and the client goes on with the master it had
 */
TEST(MasterUrlSetting, RefusesAnotherScheme) {
  EXPECT_THROW(requireMasterUrl(UNKNOWN_SCHEME_MASTER_URL),
               engine::CommandRefusedException);
}

/**
 * Given a url whose port is 0, outside 1 to 65535
 * When the setting checks it
 * Then it is refused, and the client goes on with the master it had
 */
TEST(MasterUrlSetting, RefusesAPortOutsideTheRange) {
  EXPECT_THROW(requireMasterUrl(PORT_ZERO_MASTER_URL),
               engine::CommandRefusedException);
}

}  // namespace rtype::client
