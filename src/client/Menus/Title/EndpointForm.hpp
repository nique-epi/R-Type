#pragma once

#include <functional>
#include "Endpoint.hpp"
#include "EndpointReading.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"

namespace rtype::client {

/**
 * @brief Adds the ADDRESS and PORT fields of a form to @p builder, filled with
 * @p shown, Enter on either running @p confirm. They must be the first two
 * fields of the form: readEndpoint() reads ADDRESS_ENTRY and PORT_ENTRY.
 * @returns @p builder, to go on with the rest of the form.
 */
MenuBuilder& addEndpointFields(MenuBuilder& builder,
                               const network::Endpoint& shown,
                               const std::function<void()>& confirm);

/**
 * @returns The server the ADDRESS and PORT fields of @p menu name, or, for an
 * address that is not IPv4 or a port outside LOWEST_PORT to HIGHEST_PORT,
 * INVALID_ADDRESS_STATUS or INVALID_PORT_STATUS as its problem.
 */
[[nodiscard]] EndpointReading readEndpoint(const Menu& menu);

}  // namespace rtype::client
