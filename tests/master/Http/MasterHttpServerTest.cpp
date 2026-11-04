#include <gtest/gtest.h>
#include <httplib.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "ApiResponse.hpp"
#include "Endpoint.hpp"
#include "HttpPortProbe.hpp"
#include "MasterException.hpp"
#include "MasterHttpServer.hpp"
#include "MasterHttpServerFixture.hpp"
#include "MasterHttpServerTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkConstants.hpp"

using rtype::master::ApiResponse;
using rtype::master::HttpPortUnavailableException;
using rtype::master::MasterException;
using rtype::master::MasterHttpServer;
using rtype::network::Endpoint;
using rtype::network::LOOPBACK_IPV4_ADDRESS;

/**
 * Given a route added under the API before the server opens
 * When a client calls it
 * Then it gets the status and the body of the route, as JSON
 */
TEST_F(MasterHttpServerFixture, ARouteAnswersItsStatusAndBodyAsJson) {
  addTestRoute();
  openServer();

  const httplib::Result answer = get(TEST_ROUTE_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_OK);
  EXPECT_EQ(answer->body, TEST_ROUTE_BODY);
  EXPECT_EQ(answer->get_header_value(std::string{CONTENT_TYPE_HEADER}),
            EXPECTED_JSON_TYPE);
}

/**
 * Given an open server
 * When a client calls a path of the API no route answers
 * Then it gets a problem document of Not Found
 */
TEST_F(MasterHttpServerFixture, AnUnknownPathGetsAProblemDocumentOfNotFound) {
  openServer();

  const httplib::Result answer = get(UNKNOWN_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_NOT_FOUND);
  EXPECT_EQ(answer->get_header_value(std::string{CONTENT_TYPE_HEADER}),
            EXPECTED_PROBLEM_TYPE);
  const nlohmann::json problem = nlohmann::json::parse(answer->body);
  EXPECT_EQ(problem.at(PROBLEM_STATUS_MEMBER_NAME), EXPECTED_NOT_FOUND);
  EXPECT_EQ(problem.at(PROBLEM_TITLE_MEMBER_NAME), EXPECTED_NOT_FOUND_TITLE);
}

/**
 * Given a route that throws
 * When a client calls it
 * Then it gets a problem document of Internal Server Error, which does not
 * hold the reason of the failure
 */
TEST_F(MasterHttpServerFixture, AFailingRouteHidesItsReasonFromTheClient) {
  server().addGet(TEST_ROUTE, []() -> ApiResponse {
    throw MasterException(std::string{SECRET_REASON});
  });
  openServer();

  const httplib::Result answer = get(TEST_ROUTE_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_INTERNAL_SERVER_ERROR);
  EXPECT_EQ(nlohmann::json::parse(answer->body).at(PROBLEM_TITLE_MEMBER_NAME),
            EXPECTED_INTERNAL_SERVER_ERROR_TITLE);
  EXPECT_EQ(answer->body.find(SECRET_REASON), std::string::npos);
}

/**
 * Given a route of an open server
 * When a client sends it a body one byte over 64 KiB
 * Then it gets a problem document of Payload Too Large, so the body is never
 * kept whole
 */
TEST_F(MasterHttpServerFixture, ABodyOverTheLimitIsRefusedAsTooLarge) {
  addTestRoute();
  openServer();

  const httplib::Result answer =
      getWithBody(TEST_ROUTE_URL, std::string(OVERSIZED_BODY_BYTES, 'x'));

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_PAYLOAD_TOO_LARGE);
  EXPECT_EQ(answer->get_header_value(std::string{CONTENT_TYPE_HEADER}),
            EXPECTED_PROBLEM_TYPE);
}

/**
 * Given an open server
 * When a second server opens on the same address and port
 * Then it is refused with the error that names the port taken
 */
TEST_F(MasterHttpServerFixture, ASecondServerOnATakenPortIsRefused) {
  openServer();
  MasterHttpServer second = quietHttpServer();

  EXPECT_THROW(
      static_cast<void>(second.open(Endpoint{
          .address = std::string{LOOPBACK_IPV4_ADDRESS}, .port = port()})),
      HttpPortUnavailableException);
}

/**
 * Given a server a client keeps a connection open to
 * When the server closes
 * Then close() returns and another server can open the same port
 */
TEST_F(MasterHttpServerFixture, ClosingFreesThePortWhileAClientStaysConnected) {
  addTestRoute();
  openServer();
  const std::unique_ptr<httplib::Client> client =
      keptAliveClient(port(), TEST_ROUTE_URL);
  ASSERT_NE(client, nullptr);

  server().close();

  MasterHttpServer second = quietHttpServer();
  EXPECT_EQ(second.open(Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS},
                                 .port = port()}),
            port());
}

/**
 * Given an open server
 * When it is closed
 * Then it does not report a failure: only a server that stops on its own does
 */
TEST_F(MasterHttpServerFixture, ClosingIsNotReportedAsAFailure) {
  openServer();

  server().close();

  EXPECT_EQ(failureCount(), 0);
}
