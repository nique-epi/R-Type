#pragma once

#include <functional>
#include <optional>
#include <span>
#include <string_view>
#include "ChatEntry.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

/**
 * @brief What paintChat() paints: the lines of the chat, the settings of its
 * look, the palette of the names, the text typed while the chat is open, how
 * many lines the open chat is scrolled up by, and the row the chat ends on.
 *
 * A std::reference_wrapper has no default constructor, so this struct is only
 * ever built with designated initializers naming every member without a
 * default.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ChatView {
  std::span<const ChatEntry> entries;
  std::reference_wrapper<const PlayerSettings> settings;
  std::reference_wrapper<const PlayerPalette> palette;
  /** @brief The text typed, while the chat is open; none while closed. */
  std::optional<std::string_view> draft;
  int scroll = 0;
  int bottom = 0;
};

}  // namespace rtype::client
