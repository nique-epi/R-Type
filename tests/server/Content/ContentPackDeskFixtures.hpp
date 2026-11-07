#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "ApiRequest.hpp"

namespace rtype::server::testing {

/** @returns A pack of the drifter's sheet and the Bydomancer's sprites. */
[[nodiscard]] std::vector<std::byte> somePack();

/** @returns A request from @p address for the pack @p fingerprint. */
[[nodiscard]] network::ApiRequest packRequest(std::string_view address,
                                              std::string_view fingerprint);

}  // namespace rtype::server::testing
