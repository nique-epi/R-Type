#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "GameTicket.hpp"
#include "ITicketLink.hpp"
#include "TicketReading.hpp"

/**
 * @brief A ticket link a test answers by hand, as a master answers on a later
 * frame: an ask gets no answer until answerLastAsk(), and a newer ask makes
 * an earlier answer stale, as the real link serves the newest ask only.
 */
class FakeTicketLink final : public rtype::client::ITicketLink {
 public:
  std::uint64_t askTicket(const std::string& token) override;
  [[nodiscard]] std::optional<rtype::client::TicketReading> ticket()
      const override;

  /** @brief Answers the newest ask with @p ticket; none is a master that
   * refused or did not answer. */
  void answerLastAsk(std::optional<rtype::network::GameTicket> ticket);

  /** @returns The token of each ask, in order. */
  [[nodiscard]] const std::vector<std::string>& askedTokens() const;

 private:
  std::vector<std::string> askedTokens_;
  std::optional<rtype::client::TicketReading> reading_;
};
