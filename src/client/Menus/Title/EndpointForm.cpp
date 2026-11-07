#include "EndpointForm.hpp"
#include <functional>
#include <optional>
#include <string>
#include "ConnectionScreenConstants.hpp"
#include "Endpoint.hpp"
#include "EndpointReading.hpp"
#include "EndpointText.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "NetworkException.hpp"
#include "TextField.hpp"

namespace rtype::client {

MenuBuilder& addEndpointFields(MenuBuilder& builder,
                               const network::Endpoint& shown,
                               const std::function<void()>& confirm) {
  return builder
      .textField({.label = ADDRESS_LABEL, .description = ADDRESS_DESCRIPTION},
                 TextField(shown.address, ADDRESS_FIELD_SIZE,
                           TextField::Characters::DigitsAndDots),
                 confirm)
      .textField({.label = PORT_LABEL, .description = PORT_DESCRIPTION},
                 TextField(std::to_string(shown.port), PORT_FIELD_SIZE,
                           TextField::Characters::Digits),
                 confirm);
}

EndpointReading readEndpoint(const Menu& menu) {
  try {
    return EndpointReading{
        .endpoint =
            network::Endpoint{
                .address =
                    network::ipv4AddressFromText(menu.fieldText(ADDRESS_ENTRY)),
                .port = network::portFromText(menu.fieldText(PORT_ENTRY))},
        .problem = {}};
  } catch (const network::InvalidIpv4AddressException&) {
    return EndpointReading{.endpoint = std::nullopt,
                           .problem = INVALID_ADDRESS_STATUS};
  } catch (const network::InvalidPortException&) {
    return EndpointReading{.endpoint = std::nullopt,
                           .problem = INVALID_PORT_STATUS};
  }
}

}  // namespace rtype::client
