#include "StandardTerminal.hpp"
#include <iostream>
#include <string_view>
#include "SavedTerminalModes.hpp"  // IWYU pragma: keep

namespace rtype::server {

StandardTerminal::StandardTerminal() = default;

StandardTerminal::~StandardTerminal() { leaveTypingMode(); }

void StandardTerminal::write(std::string_view text) {
  std::cout << text << std::flush;
}

}  // namespace rtype::server
