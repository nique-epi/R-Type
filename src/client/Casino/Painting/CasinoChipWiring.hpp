#pragma once

#include <functional>
#include "ISoundPlayer.hpp"
#include "Language.hpp"

namespace rtype::client {

/** @brief How a chip of the casino is wired into its screen: what it runs,
 * the sounds its click plays through, which must outlive it, and the language
 * its label is written in. */
struct CasinoChipWiring {
  std::function<void()> action;
  std::reference_wrapper<ISoundPlayer> sounds;
  Language language{};
};

}  // namespace rtype::client
