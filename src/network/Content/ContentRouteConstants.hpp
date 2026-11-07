#pragma once

#include <string_view>
#include "MasterRoute.hpp"
#include "RouteMethod.hpp"

namespace rtype::network {

/**
 * @brief The content pack a supervisor plays with, named by its fingerprint
 * in hexadecimal; open to everyone, answered with the bytes of the pack as
 * OCTET_STREAM_CONTENT_TYPE, or 404 for any other fingerprint.
 */
constexpr MasterRoute CONTENT_PACK_ROUTE{.method = RouteMethod::Get,
                                         .path = "/packs/:fingerprint"};

/** @brief The parameter of CONTENT_PACK_ROUTE that names the pack. */
constexpr std::string_view FINGERPRINT_PATH_PARAMETER = "fingerprint";

}  // namespace rtype::network
