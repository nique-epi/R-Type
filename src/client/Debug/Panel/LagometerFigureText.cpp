#include "LagometerFigureText.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "LagometerConstants.hpp"

namespace rtype::client {

std::string figureText(std::optional<std::uint64_t> figure) {
  return figure.has_value() ? std::to_string(*figure)
                            : std::string{NO_FIGURE_TEXT};
}

std::uint64_t wholeMilliseconds(std::chrono::microseconds duration) {
  return static_cast<std::uint64_t>(
      std::chrono::round<std::chrono::milliseconds>(duration).count());
}

}  // namespace rtype::client
