#include <gtest/gtest.h>
#include <string>
#include "Endpoint.hpp"
#include "MasterUrl.hpp"
#include "MasterUrlTestConstants.hpp"

namespace rtype::client {

using network::Endpoint;
using testing::ADDRESSED_MASTER_ADDRESS;
using testing::ADDRESSED_MASTER_PORT;
using testing::ADDRESSED_MASTER_URL;

/**
 * Given a master named by its IPv4 address and port
 * When its url is written
 * Then it is http:// followed by the address, a colon and the port
 */
TEST(MasterUrl, AMasterIsWrittenAsAnHttpUrl) {
  EXPECT_EQ(
      masterUrlOf(Endpoint{.address = std::string{ADDRESSED_MASTER_ADDRESS},
                           .port = ADDRESSED_MASTER_PORT}),
      ADDRESSED_MASTER_URL);
}

}  // namespace rtype::client
