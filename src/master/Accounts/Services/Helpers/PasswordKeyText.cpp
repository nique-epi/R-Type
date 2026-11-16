#include "PasswordKeyText.hpp"
#include <optional>
#include <string_view>
#include "Hashing.hpp"
#include "MasterConstants.hpp"
#include "MasterException.hpp"
#include "PasswordKey.hpp"

namespace rtype::master {

network::PasswordKey passwordKeyFromText(std::string_view text) {
  if (text.empty()) {
    throw MissingPasswordKeyException(PASSWORD_KEY_VARIABLE);
  }
  const std::optional<network::PasswordKey> key =
      network::passwordKeyOfHexadecimal(text);
  if (!key.has_value()) {
    throw InvalidPasswordKeyException(PASSWORD_KEY_VARIABLE);
  }
  return *key;
}

}  // namespace rtype::master
