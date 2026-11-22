#include "EndpointForm.hpp"
#include <functional>
#include <optional>
#include <string>
#include "ConnectionScreenConstants.hpp"
#include "Endpoint.hpp"
#include "EndpointReading.hpp"
#include "EndpointText.hpp"
#include "Language.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "NetworkException.hpp"
#include "ShownEndpoint.hpp"
#include "TextField.hpp"

namespace rtype::client {

MenuBuilder& addEndpointFields(MenuBuilder& builder, const ShownEndpoint& shown,
                               const std::function<void()>& confirm) {
  const Language language = shown.language;
  return builder
      .textField({.label = std::string(ADDRESS_LABEL.in(language)),
                  .description = std::string(ADDRESS_DESCRIPTION.in(language))},
                 TextField(shown.endpoint.address, ADDRESS_FIELD_SIZE,
                           TextField::Characters::DigitsAndDots),
                 confirm)
      .textField({.label = std::string(PORT_LABEL.in(language)),
                  .description = std::string(PORT_DESCRIPTION.in(language))},
                 TextField(std::to_string(shown.endpoint.port), PORT_FIELD_SIZE,
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
        .problem = std::nullopt};
  } catch (const network::InvalidIpv4AddressException&) {
    return EndpointReading{.endpoint = std::nullopt,
                           .problem = INVALID_ADDRESS_STATUS};
  } catch (const network::InvalidPortException&) {
    return EndpointReading{.endpoint = std::nullopt,
                           .problem = INVALID_PORT_STATUS};
  }
}

}  // namespace rtype::client
