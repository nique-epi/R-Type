#include "StandardOutput.hpp"
#include <iostream>
#include <string_view>

namespace rtype::server {

void StandardOutput::write(std::string_view line) {
  std::cout << line << '\n';
  std::cout.flush();
}

}  // namespace rtype::server
