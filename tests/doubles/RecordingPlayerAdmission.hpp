#pragma once

#include <cstdint>
#include <vector>
#include "Endpoint.hpp"
#include "IPlayerAdmission.hpp"

/** @brief One call to admit() or forget(). */
struct AdmissionCall {
  rtype::network::Endpoint player;
  std::uint64_t sessionToken{};

  bool operator==(const AdmissionCall&) const = default;
};

/** @brief Player admission double that keeps every call it receives, in
 * order, and admits nobody. */
class RecordingPlayerAdmission : public rtype::server::IPlayerAdmission {
 public:
  void admit(const rtype::network::Endpoint& player,
             std::uint64_t sessionToken) override;
  void forget(const rtype::network::Endpoint& player,
              std::uint64_t sessionToken) override;

  [[nodiscard]] const std::vector<AdmissionCall>& admitted() const;
  [[nodiscard]] const std::vector<AdmissionCall>& forgotten() const;

 private:
  std::vector<AdmissionCall> admitted_;
  std::vector<AdmissionCall> forgotten_;
};
