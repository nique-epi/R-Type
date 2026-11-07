#include "InputRedundancyFixture.hpp"
#include <cstdint>
#include <vector>
#include "InputRedundancyTestConstants.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "SessionsFixtures.hpp"

namespace rtype::server::testing {

InputRedundancyFixture::InputRedundancyFixture()
    : player_(addPlayer(roster_)),
      keeper_(roster_, recorderOf(shown_)),
      loss_(LOSS_SEED, TEN_PERCENT_LOSS) {}

void InputRedundancyFixture::sendInputs(std::uint32_t count) {
  for (std::uint32_t step = 1; step <= count; ++step) {
    sendInput(step);
    confirmWhenDue(step);
  }
}

void InputRedundancyFixture::sendInput(std::uint32_t number) {
  const network::PlayerInput message =
      sender_.add(static_cast<std::uint8_t>(number & ACTION_BITS_MASK),
                  network::NO_SHOWN_TICK);
  if (!loss_.losesNext()) {
    keeper_.keepNewerOf(originOf(0), message);
  }
}

void InputRedundancyFixture::confirmWhenDue(std::uint32_t step) {
  if (step % INPUTS_BETWEEN_CONFIRMATIONS == 0 && !loss_.losesNext()) {
    confirmationsInFlight_.emplace_back(step + INPUTS_A_CONFIRMATION_TAKES,
                                        player_.newestInputNumber.value_or(0));
  }
  while (!confirmationsInFlight_.empty() &&
         confirmationsInFlight_.front().first <= step) {
    sender_.confirmUpTo(confirmationsInFlight_.front().second);
    confirmationsInFlight_.pop_front();
  }
}

const std::vector<ShownInput>& InputRedundancyFixture::shown() const {
  return shown_;
}

}  // namespace rtype::server::testing
