#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "EndpointText.hpp"
#include "EndpointTextTestConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"

using rtype::network::HIGHEST_PORT;
using rtype::network::InvalidIpv4AddressException;
using rtype::network::InvalidPortException;
using rtype::network::ipv4AddressFromText;
using rtype::network::LOWEST_PORT;
using rtype::network::portFromText;

/**
 * Given a port written in decimal digits
 * When it is read
 * Then the port it names is returned
 */
TEST(EndpointText, ReadsThePortWrittenInDigits) {
  EXPECT_EQ(portFromText(PORT_TEXT), PORT_OF_PORT_TEXT);
}

/**
 * Given the lowest and the highest port, written in digits
 * When they are read
 * Then both are returned
 */
TEST(EndpointText, AcceptsTheLowestAndTheHighestPort) {
  EXPECT_EQ(portFromText(std::to_string(LOWEST_PORT)), LOWEST_PORT);
  EXPECT_EQ(portFromText(std::to_string(HIGHEST_PORT)), HIGHEST_PORT);
}

/**
 * Given port 0, which asks the system for any free port
 * When it is read
 * Then it is refused
 */
TEST(EndpointText, RefusesPortZero) {
  EXPECT_THROW(static_cast<void>(portFromText("0")), InvalidPortException);
}

/**
 * Given numbers above the highest port
 * When they are read
 * Then they are refused
 */
TEST(EndpointText, RefusesNumbersAboveTheHighestPort) {
  for (const std::string_view text : PORTS_ABOVE_THE_HIGHEST) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(portFromText(text)), InvalidPortException);
  }
}

/**
 * Given an empty text
 * When it is read as a port
 * Then it is refused
 */
TEST(EndpointText, RefusesAnEmptyPort) {
  EXPECT_THROW(static_cast<void>(portFromText("")), InvalidPortException);
}

/**
 * Given digits with a sign or a space around them
 * When they are read as a port
 * Then they are refused
 */
TEST(EndpointText, RefusesASignOrASpaceAroundThePort) {
  for (const std::string_view text : PORTS_WITH_SIGN_OR_SPACE) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(portFromText(text)), InvalidPortException);
  }
}

/**
 * Given digits followed by other characters
 * When they are read as a port
 * Then they are refused, even though they start with a valid port
 */
TEST(EndpointText, RefusesTextAfterTheDigitsOfThePort) {
  for (const std::string_view text : PORTS_FOLLOWED_BY_OTHER_TEXT) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(portFromText(text)), InvalidPortException);
  }
}

/**
 * Given an IPv4 address written as four numbers
 * When it is read
 * Then the same text is returned
 */
TEST(EndpointText, ReturnsAnIpv4AddressUnchanged) {
  EXPECT_EQ(ipv4AddressFromText(LOCAL_NETWORK_ADDRESS),
            std::string(LOCAL_NETWORK_ADDRESS));
}

/**
 * Given the addresses made of the smallest and of the largest numbers
 * When they are read
 * Then both are accepted
 */
TEST(EndpointText, AcceptsTheSmallestAndTheLargestNumbers) {
  for (const std::string_view text : IPV4_ADDRESSES_AT_THE_BOUNDS) {
    SCOPED_TRACE(text);
    EXPECT_EQ(ipv4AddressFromText(text), std::string(text));
  }
}

/**
 * Given addresses holding a number above 255
 * When they are read
 * Then they are refused
 */
TEST(EndpointText, RefusesANumberAbove255) {
  for (const std::string_view text : IPV4_ADDRESSES_WITH_A_NUMBER_ABOVE_255) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(ipv4AddressFromText(text)),
                 InvalidIpv4AddressException);
  }
}

/**
 * Given addresses of three and of five numbers
 * When they are read
 * Then they are refused
 */
TEST(EndpointText, RefusesThreeOrFiveNumbers) {
  for (const std::string_view text :
       IPV4_ADDRESSES_WITH_THREE_OR_FIVE_NUMBERS) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(ipv4AddressFromText(text)),
                 InvalidIpv4AddressException);
  }
}

/**
 * Given texts where a number is missing between, before or after the dots
 * When they are read as an address
 * Then they are refused
 */
TEST(EndpointText, RefusesAnEmptyNumber) {
  for (const std::string_view text : IPV4_ADDRESSES_WITH_AN_EMPTY_NUMBER) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(ipv4AddressFromText(text)),
                 InvalidIpv4AddressException);
  }
}

/**
 * Given addresses where a number other than 0 starts with 0
 * When they are read
 * Then they are refused, since some parsers read such a number in octal
 */
TEST(EndpointText, RefusesANumberStartingWithZero) {
  for (const std::string_view text : IPV4_ADDRESSES_WITH_A_LEADING_ZERO) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(ipv4AddressFromText(text)),
                 InvalidIpv4AddressException);
  }
}

/**
 * Given addresses with a sign, a space or a port around the numbers
 * When they are read
 * Then they are refused
 */
TEST(EndpointText, RefusesOtherTextAroundTheAddress) {
  for (const std::string_view text : IPV4_ADDRESSES_WITH_OTHER_TEXT) {
    SCOPED_TRACE(text);
    EXPECT_THROW(static_cast<void>(ipv4AddressFromText(text)),
                 InvalidIpv4AddressException);
  }
}

/**
 * Given a host name
 * When it is read as an address
 * Then it is refused instead of being resolved
 */
TEST(EndpointText, RefusesAHostName) {
  EXPECT_THROW(static_cast<void>(ipv4AddressFromText(HOST_NAME)),
               InvalidIpv4AddressException);
}

/**
 * Given an IPv6 address
 * When it is read as an IPv4 address
 * Then it is refused
 */
TEST(EndpointText, RefusesAnIpv6Address) {
  EXPECT_THROW(static_cast<void>(ipv4AddressFromText(IPV6_LOOPBACK_ADDRESS)),
               InvalidIpv4AddressException);
}
