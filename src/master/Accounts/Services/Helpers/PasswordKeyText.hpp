#pragma once

#include <string_view>
#include "PasswordKey.hpp"

namespace rtype::master {

/**
 * @returns The key of the passwords @p text writes, as PASSWORD_KEY_VARIABLE
 * holds it: network::PASSWORD_KEY_BYTES in hexadecimal. The message of an
 * exception names the variable, never its value.
 * @throws MissingPasswordKeyException for an empty text.
 * @throws InvalidPasswordKeyException for any other text that is no key.
 */
[[nodiscard]] network::PasswordKey passwordKeyFromText(std::string_view text);

}  // namespace rtype::master
