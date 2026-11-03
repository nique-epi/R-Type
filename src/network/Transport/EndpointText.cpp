#include "EndpointText.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "WholeNumberText.hpp"

namespace rtype::network {

namespace {

bool isIpv4AddressPart(std::string_view part) {
  if (part.size() > 1 && part.front() == '0') {
    return false;
  }
  const std::optional<unsigned int> value = wholeNumberIn<unsigned int>(part);
  return value.has_value() && *value <= IPV4_ADDRESS_PART_MAXIMUM;
}

bool isIpv4Address(std::string_view text) {
  const auto separatorCount = static_cast<std::size_t>(
      std::ranges::count(text, IPV4_ADDRESS_SEPARATOR));
  if (separatorCount != IPV4_ADDRESS_PART_COUNT - 1) {
    return false;
  }
  std::string_view rest = text;
  for (std::size_t part = 0; part < IPV4_ADDRESS_PART_COUNT; ++part) {
    const std::size_t separator = rest.find(IPV4_ADDRESS_SEPARATOR);
    if (!isIpv4AddressPart(rest.substr(0, separator))) {
      return false;
    }
    rest.remove_prefix(separator == std::string_view::npos ? rest.size()
                                                           : separator + 1);
  }
  return true;
}

}  // namespace

std::uint16_t portFromText(std::string_view text) {
  const std::optional<std::uint16_t> port = wholeNumberIn<std::uint16_t>(text);
  if (!port.has_value() || *port < LOWEST_PORT) {
    throw InvalidPortException(std::string(text));
  }
  return *port;
}

std::string ipv4AddressFromText(std::string_view text) {
  if (!isIpv4Address(text)) {
    throw InvalidIpv4AddressException(std::string(text));
  }
  return std::string(text);
}

}  // namespace rtype::network
