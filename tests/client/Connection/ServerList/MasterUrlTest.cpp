#include <gtest/gtest.h>
#include <string>
#include "Endpoint.hpp"
#include "MasterUrl.hpp"
#include "MasterUrlTestConstants.hpp"

namespace rtype::client {

using network::Endpoint;
using testing::ADDRESSED_MASTER_ADDRESS;
using testing::ADDRESSED_MASTER_PORT;
using testing::ADDRESSED_MASTER_TEXT;
using testing::ADDRESSED_MASTER_URL;
using testing::NAMED_MASTER_URL;

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

/**
 * Given the url of a master named by its IPv4 address and port
 * When the form reads it
 * Then it is that address and that port
 */
TEST(MasterUrl, AnAddressedUrlReadsBackAsItsEndpoint) {
  EXPECT_EQ(masterEndpointOf(ADDRESSED_MASTER_URL).value_or(Endpoint{}),
            (Endpoint{.address = std::string{ADDRESSED_MASTER_ADDRESS},
                      .port = ADDRESSED_MASTER_PORT}));
}

/**
 * Given the url of a master named by host
 * When the form reads it
 * Then it reads no endpoint, since no field can hold a host name
 */
TEST(MasterUrl, ANamedUrlReadsNoEndpoint) {
  EXPECT_FALSE(masterEndpointOf(NAMED_MASTER_URL).has_value());
}

/**
 * Given the url of a master
 * When the server map writes it for the player
 * Then it is the url without http://
 */
TEST(MasterUrl, APlayerReadsTheUrlWithoutItsScheme) {
  EXPECT_EQ(masterTextOf(ADDRESSED_MASTER_URL), ADDRESSED_MASTER_TEXT);
}

}  // namespace rtype::client
