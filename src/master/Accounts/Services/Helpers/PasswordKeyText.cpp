#include "PasswordKeyText.hpp"
#include <optional>
#include <string_view>
#include "Hashing.hpp"
#include "MasterConstants.hpp"
#include "MasterException.hpp"
#include "PasswordKey.hpp"

namespace rtype::master {

std::optional<network::PasswordKey> passwordKeyFromText(std::string_view text) {
  if (text.empty()) {
    return std::nullopt;
  }
  const std::optional<network::PasswordKey> key =
      network::passwordKeyOfHexadecimal(text);
  if (!key.has_value()) {
    throw InvalidPasswordKeyException(PASSWORD_KEY_VARIABLE);
  }
  return key;
}

}  // namespace rtype::master
