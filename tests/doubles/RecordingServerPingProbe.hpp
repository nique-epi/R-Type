#pragma once

#include <vector>
#include "Endpoint.hpp"
#include "IServerPingProbe.hpp"
#include "PingAnswer.hpp"

/**
 * @brief Ping probe double: it writes down the servers of each round, and
 * hands over the answers the test delivers, once each, as the real probe hands
 * each answer once. A server the test never answers for stays silent, as an
 * unreachable server does.
 */
class RecordingServerPingProbe final : public rtype::client::IServerPingProbe {
 public:
  void measure(std::vector<rtype::network::Endpoint> servers) override;
  [[nodiscard]] std::vector<rtype::client::PingAnswer> answers() override;

  /** @brief Leaves @p answer for the next answers(). */
  void deliver(rtype::client::PingAnswer answer);

  /** @returns The servers of each round, in order. */
  [[nodiscard]] const std::vector<std::vector<rtype::network::Endpoint>>&
  rounds() const;

 private:
  std::vector<std::vector<rtype::network::Endpoint>> rounds_;
  std::vector<rtype::client::PingAnswer> delivered_;
};
