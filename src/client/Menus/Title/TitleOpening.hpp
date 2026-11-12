#pragma once

#include "IUpdateNotice.hpp"
#include "ScreenId.hpp"
#include "SquadronStart.hpp"

namespace rtype::client {

/** @brief How the menus module opens a title: the identifier it is built
 * under, how its squadron starts, and the update module, or null without
 * one. */
struct TitleOpening {
  ScreenId screen;
  SquadronStart squadronStart{};
  const IUpdateNotice* notice{};
};

}  // namespace rtype::client
