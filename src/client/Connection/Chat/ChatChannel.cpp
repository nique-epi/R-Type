#include "ChatChannel.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "ChatChannelConstants.hpp"
#include "ChatEntry.hpp"
#include "ChatLine.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PlayerChatLine.hpp"
#include "TimeConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/** @returns A line the client writes itself, saying @p text. */
ChatEntry clientLine(std::string_view text) {
  return ChatEntry{.playerNumber = CLIENT_WRITER,
                   .nickname = {},
                   .text = std::string{text},
                   .age = {}};
}

}  // namespace

ChatChannel::ChatChannel(network::MessageDispatcher& dispatcher,
                         const IWaitingRoom& room)
    : room_(&room) {
  dispatcher.registerHandler<network::PlayerChatLine>(
      [this](const network::MessageOrigin&,
             const network::PlayerChatLine& relayed) { receive(relayed); });
}

bool ChatChannel::send(std::string_view text) {
  if (outbox_ == nullptr || !network::isValidChatText(text)) {
    return false;
  }
  if (waiting_.size() >= CHAT_LINES_WAITING) {
    keep(clientLine(CHAT_QUEUE_FULL_TEXT));
    return false;
  }
  waiting_.emplace_back(text);
  if (!inFlight_.has_value()) {
    sendNextLine();
  }
  return true;
}

std::span<const ChatEntry> ChatChannel::entries() const { return entries_; }

void ChatChannel::sessionOpened(const network::LobbyPlayer& ownPlayer,
                                ISessionOutbox& outbox) {
  outbox_ = &outbox;
  ownNumber_ = ownPlayer.playerNumber;
  nextLineNumber_ = FIRST_CHAT_LINE_NUMBER;
}

void ChatChannel::sessionClosed() {
  outbox_ = nullptr;
  waiting_.clear();
  inFlight_.reset();
  entries_.clear();
}

void ChatChannel::sessionHeld(engine::Duration elapsed) {
  for (ChatEntry& entry : entries_) {
    entry.age += elapsed;
  }
  followLineInFlight(elapsed);
}

void ChatChannel::receive(const network::PlayerChatLine& relayed) {
  if (outbox_ == nullptr) {
    return;
  }
  const bool ownLineInFlight = inFlight_.has_value() &&
                               relayed.playerNumber == ownNumber_ &&
                               relayed.line.lineNumber == inFlight_->lineNumber;
  keep(ChatEntry{.playerNumber = relayed.playerNumber,
                 .nickname = nicknameOf(relayed.playerNumber),
                 .text = relayed.line.text,
                 .age = {}});
  if (ownLineInFlight) {
    inFlight_.reset();
    sendNextLine();
  }
}

void ChatChannel::sendNextLine() {
  if (waiting_.empty()) {
    return;
  }
  inFlight_ = network::ChatLine{.lineNumber = nextLineNumber_,
                                .text = std::move(waiting_.front())};
  waiting_.pop_front();
  ++nextLineNumber_;
  sinceSent_ = engine::Duration::zero();
  sinceFirstSent_ = engine::Duration::zero();
  sendMessage(*outbox_, *inFlight_);
}

void ChatChannel::followLineInFlight(engine::Duration elapsed) {
  if (!inFlight_.has_value()) {
    return;
  }
  sinceSent_ += elapsed;
  sinceFirstSent_ += elapsed;
  if (sinceFirstSent_ >= CHAT_GIVE_UP_TIME) {
    inFlight_.reset();
    keep(clientLine(UNANSWERED_CHAT_TEXT));
    sendNextLine();
  } else if (sinceSent_ >= CHAT_LINE_INTERVAL) {
    sendMessage(*outbox_, *inFlight_);
    sinceSent_ = engine::Duration::zero();
  }
}

void ChatChannel::keep(ChatEntry entry) {
  entries_.push_back(std::move(entry));
  if (entries_.size() > CHAT_LINES_KEPT) {
    entries_.erase(entries_.begin());
  }
}

std::string ChatChannel::nicknameOf(std::uint8_t playerNumber) const {
  const std::span<const network::LobbyPlayer> players = room_->lobbyPlayers();
  const auto found = std::ranges::find(players, playerNumber,
                                       &network::LobbyPlayer::playerNumber);
  return found == players.end() ? std::string{} : found->nickname;
}

}  // namespace rtype::client
