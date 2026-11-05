#include <gtest/gtest.h>
#include <httplib.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "Endpoint.hpp"
#include "HttpHeader.hpp"
#include "HttpPortProbe.hpp"
#include "MasterException.hpp"
#include "MasterHttpServer.hpp"
#include "MasterHttpServerFixture.hpp"
#include "MasterHttpServerTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkConstants.hpp"

using rtype::master::ApiRequest;
using rtype::master::ApiResponse;
using rtype::master::HttpHeader;
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
  EXPECT_EQ(problem.at(PROBLEM_STATUS_MEMBER_NAME).get<int>(),
            EXPECTED_NOT_FOUND);
  EXPECT_EQ(problem.at(PROBLEM_TITLE_MEMBER_NAME).get<std::string>(),
            EXPECTED_NOT_FOUND_TITLE);
}

/**
 * Given a route that throws
 * When a client calls it
 * Then it gets a problem document of Internal Server Error, which does not
 * hold the reason of the failure
 */
TEST_F(MasterHttpServerFixture, AFailingRouteHidesItsReasonFromTheClient) {
  server().add(TEST_GET_ROUTE,
               []([[maybe_unused]] const ApiRequest& request) -> ApiResponse {
                 throw MasterException(std::string{SECRET_REASON});
               });
  openServer();

  const httplib::Result answer = get(TEST_ROUTE_URL);

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_INTERNAL_SERVER_ERROR);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(PROBLEM_TITLE_MEMBER_NAME)
                .get<std::string>(),
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

/**
 * Given a PUT route of an open server
 * When a client sends it a body
 * Then the route reads that body
 */
TEST_F(MasterHttpServerFixture, APutRouteReadsTheBodySent) {
  addEchoRoute();
  openServer();

  const httplib::Result answer = put(TEST_ROUTE_URL, {});

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(ECHO_BODY_MEMBER)
                .get<std::string>(),
            ECHOED_BODY);
}

/**
 * Given a route of an open server
 * When a client sends a key with the Bearer scheme, written in either case
 * Then the route reads the key alone
 */
TEST_F(MasterHttpServerFixture, ARouteReadsTheBearerTokenInAnyCase) {
  addEchoRoute();
  openServer();

  for (const std::string_view authorization :
       {BEARER_AUTHORIZATION, LOWERCASE_BEARER_AUTHORIZATION}) {
    SCOPED_TRACE(authorization);
    const httplib::Result answer =
        put(TEST_ROUTE_URL,
            httplib::Headers{{std::string{AUTHORIZATION_HEADER_NAME},
                              std::string{authorization}}});

    ASSERT_TRUE(answer);
    EXPECT_EQ(nlohmann::json::parse(answer->body)
                  .at(ECHO_TOKEN_MEMBER)
                  .get<std::string>(),
              SENT_TOKEN);
  }
}

/**
 * Given a route of an open server
 * When a client sends another scheme, a header too short for any scheme, or
 * the Bearer scheme with no key
 * Then the route reads no key, and still answers
 */
TEST_F(MasterHttpServerFixture, ARouteReadsNoTokenOutsideTheBearerScheme) {
  addEchoRoute();
  openServer();

  for (const std::string_view authorization :
       {BASIC_AUTHORIZATION, SHORT_AUTHORIZATION, BARE_BEARER_AUTHORIZATION}) {
    SCOPED_TRACE(authorization);
    const httplib::Result answer =
        put(TEST_ROUTE_URL,
            httplib::Headers{{std::string{AUTHORIZATION_HEADER_NAME},
                              std::string{authorization}}});

    ASSERT_TRUE(answer);
    EXPECT_EQ(answer->status, EXPECTED_OK);
    EXPECT_TRUE(
        nlohmann::json::parse(answer->body).at(ECHO_TOKEN_MEMBER).is_null());
  }
}

/**
 * Given a route of an open server
 * When a client on the loopback names another address in X-Forwarded-For
 * Then the route reads the loopback, the address of the TCP peer
 */
TEST_F(MasterHttpServerFixture,
       ARouteReadsTheAddressOfThePeerNotAForwardedOne) {
  addEchoRoute();
  openServer();

  const httplib::Result answer =
      put(TEST_ROUTE_URL, httplib::Headers{{std::string{FORWARDED_FOR_HEADER},
                                            std::string{FORGED_ADDRESS}}});

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(ECHO_ADDRESS_MEMBER)
                .get<std::string>(),
            LOOPBACK_ADDRESS);
}

/**
 * Given the echo route, which answers what it read of the request
 * When a client calls it with a query that gives limit twice and offset once
 * Then the route reads the first limit, and the offset
 */
TEST_F(MasterHttpServerFixture, ARouteReadsTheQueryOfItsPath) {
  addEchoRoute();
  openServer();

  const httplib::Result answer = put(QUERIED_ROUTE_URL, {});

  ASSERT_TRUE(answer);
  EXPECT_EQ(nlohmann::json::parse(answer->body).at(ECHO_QUERY_MEMBER),
            nlohmann::json::parse(EXPECTED_QUERY_ECHO));
}

/**
 * Given a route that refuses a request with a detail
 * When a client calls it
 * Then it gets a problem document of Bad Request that carries the detail
 */
TEST_F(MasterHttpServerFixture, ARefusalCarriesItsDetail) {
  server().add(TEST_PUT_ROUTE, []([[maybe_unused]] const ApiRequest& request) {
    return ApiResponse{.status = EXPECTED_BAD_REQUEST,
                       .body = {},
                       .detail = std::string{BAD_REQUEST_DETAIL},
                       .header = std::nullopt};
  });
  openServer();

  const httplib::Result answer = put(TEST_ROUTE_URL, {});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_BAD_REQUEST);
  EXPECT_EQ(answer->get_header_value(std::string{CONTENT_TYPE_HEADER}),
            EXPECTED_PROBLEM_TYPE);
  EXPECT_EQ(nlohmann::json::parse(answer->body)
                .at(PROBLEM_DETAIL_MEMBER_NAME)
                .get<std::string>(),
            BAD_REQUEST_DETAIL);
}

/**
 * Given a route that answers a creation with the place of what it created
 * When a client calls it
 * Then the answer carries that Location header
 */
TEST_F(MasterHttpServerFixture, ARouteSetsTheHeaderOfItsAnswer) {
  server().add(TEST_PUT_ROUTE, []([[maybe_unused]] const ApiRequest& request) {
    return ApiResponse{
        .status = EXPECTED_CREATED,
        .body = std::string{TEST_ROUTE_BODY},
        .detail = std::nullopt,
        .header = HttpHeader{.name = std::string{LOCATION_HEADER_NAME},
                             .value = std::string{CREATED_LOCATION}}};
  });
  openServer();

  const httplib::Result answer = put(TEST_ROUTE_URL, {});

  ASSERT_TRUE(answer);
  EXPECT_EQ(answer->status, EXPECTED_CREATED);
  EXPECT_EQ(answer->get_header_value(std::string{LOCATION_HEADER_NAME}),
            CREATED_LOCATION);
}
