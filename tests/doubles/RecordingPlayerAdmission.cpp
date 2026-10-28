#include "RecordingPlayerAdmission.hpp"
#include <cstdint>
#include <vector>
#include "Endpoint.hpp"

void RecordingPlayerAdmission::admit(const rtype::network::Endpoint& player,
                                     std::uint64_t sessionToken) {
  admitted_.push_back(
      AdmissionCall{.player = player, .sessionToken = sessionToken});
}

void RecordingPlayerAdmission::forget(const rtype::network::Endpoint& player,
                                      std::uint64_t sessionToken) {
  forgotten_.push_back(
      AdmissionCall{.player = player, .sessionToken = sessionToken});
}

const std::vector<AdmissionCall>& RecordingPlayerAdmission::admitted() const {
  return admitted_;
}

const std::vector<AdmissionCall>& RecordingPlayerAdmission::forgotten() const {
  return forgotten_;
}
