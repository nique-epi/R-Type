#include "DeviceLinksRoutesFixtures.hpp"
#include <cctype>
#include <string>
#include <string_view>
#include "MasterMessageConstants.hpp"

std::string lowercaseWithoutSeparator(std::string_view code) {
  std::string typed;
  for (const char letter : code) {
    if (letter != rtype::network::DEVICE_LINK_USER_CODE_SEPARATOR) {
      typed.push_back(
          static_cast<char>(std::tolower(static_cast<unsigned char>(letter))));
    }
  }
  return typed;
}
