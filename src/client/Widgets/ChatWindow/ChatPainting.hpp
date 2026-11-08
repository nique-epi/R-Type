#pragma once

#include <span>
#include "ChatEntry.hpp"
#include "ChatView.hpp"
#include "IInterfaceCanvas.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

/**
 * @brief Paints the chat, nothing while it is hidden. Closed: its newest
 * lines that still show, chatClosedLines at most, outlined over the picture,
 * each fading out over its last CHAT_FADE_TIME. Open: the dim layer of a
 * dialog at chatBackground percent over the whole picture, SAY and its field
 * on the bottom row, and chatOpenLines lines above, scrolled up by the view.
 */
void paintChat(IInterfaceCanvas& canvas, const ChatView& view);

/** @returns How many lines @p entries take at @p letters letters a line. */
[[nodiscard]] int chatLineCount(std::span<const ChatEntry> entries,
                                int letters);

/** @returns Whether @p entry still shows over a closed chat: younger than
 * chatSeconds. */
[[nodiscard]] bool isShownClosed(const ChatEntry& entry,
                                 const PlayerSettings& settings);

}  // namespace rtype::client
