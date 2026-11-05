#include "PlayerConnectionFixture.hpp"
#include <string>
#include <string_view>
#include "InstallContext.hpp"
#include "NetworkConditionsConstants.hpp"
#include "PlayerConnectionTestConstants.hpp"
#include "ServiceRegistry.hpp"

namespace {

bool drawsEveryType([[maybe_unused]] std::string_view name) { return true; }

}  // namespace

PlayerConnectionFixture::PlayerConnectionFixture()
    : stages_(SOME_LOOP_STAGES),
      connection_(rtype::network::PERFECT_NETWORK, drawsEveryType) {}

void PlayerConnectionFixture::provideTheParts() {
  rtype::engine::InstallContext context{std::string{PROVIDING_MODULE_NAME},
                                        services_, stages_};
  connection_.provideParts(context);
}

const rtype::engine::ServiceRegistry& PlayerConnectionFixture::services()
    const {
  return services_;
}
