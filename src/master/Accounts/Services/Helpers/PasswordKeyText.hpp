#pragma once

#include <optional>
#include <string_view>
#include "PasswordKey.hpp"

namespace rtype::master {

/**
 * @returns The key of the passwords @p text writes, as PASSWORD_KEY_VARIABLE
 * holds it: nothing for an empty text, which runs the master without accounts.
 * @throws InvalidPasswordKeyException for any other text that is not
 * network::PASSWORD_KEY_BYTES in hexadecimal; the message never shows it.
 */
[[nodiscard]] std::optional<network::PasswordKey> passwordKeyFromText(
    std::string_view text);

}  // namespace rtype::master
