#pragma once

#include "EncodedMessage.hpp"

namespace rtype::server::testing {

/** @returns The PlayerLeft of the second player, encoded. */
[[nodiscard]] network::EncodedMessage playerLeftMessage();

/** @returns The EntityDestroyed of a killed monster, encoded. */
[[nodiscard]] network::EncodedMessage entityDestroyedMessage();

}  // namespace rtype::server::testing
