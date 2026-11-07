#include "RecordingChatChannel.hpp"
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ChatEntry.hpp"

using rtype::client::ChatEntry;

bool RecordingChatChannel::send(std::string_view text) {
  sent_.emplace_back(text);
  return true;
}

std::span<const ChatEntry> RecordingChatChannel::entries() const {
  return entries_;
}

void RecordingChatChannel::receive(ChatEntry entry) {
  entries_.push_back(std::move(entry));
}

const std::vector<std::string>& RecordingChatChannel::sent() const {
  return sent_;
}
