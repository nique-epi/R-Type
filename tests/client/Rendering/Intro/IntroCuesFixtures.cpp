#include "IntroCuesFixtures.hpp"
#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>
#include "IntroCue.hpp"

using rtype::client::IntroCue;

std::size_t cuesPlaying(const std::vector<IntroCue>& cues,
                        std::string_view sound) {
  return static_cast<std::size_t>(
      std::ranges::count(cues, sound, &IntroCue::sound));
}
