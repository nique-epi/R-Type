#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ChatEntry.hpp"
#include "IChatChannel.hpp"

/** @brief Chat double that writes down the lines sent and shows the lines the
 * test receives: a real channel changes its lines at any frame. */
class RecordingChatChannel : public rtype::client::IChatChannel {
 public:
  bool send(std::string_view text) override;
  [[nodiscard]] std::span<const rtype::client::ChatEntry> entries()
      const override;

  /** @brief Adds a line, as if the server relayed it. */
  void receive(rtype::client::ChatEntry entry);

  [[nodiscard]] const std::vector<std::string>& sent() const;

 private:
  std::vector<rtype::client::ChatEntry> entries_;
  std::vector<std::string> sent_;
};
