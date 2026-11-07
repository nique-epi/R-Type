#include "PlayerConnectionFixture.hpp"
#include <string>
#include "AssetFolder.hpp"
#include "ContentPlaces.hpp"
#include "EntityAppearances.hpp"
#include "InstallContext.hpp"
#include "NetworkConditionsConstants.hpp"
#include "PlayerConnectionTestConstants.hpp"
#include "ServiceRegistry.hpp"
#include "SessionEntityTypesFixtures.hpp"

PlayerConnectionFixture::PlayerConnectionFixture()
    : stages_(SOME_LOOP_STAGES),
      appearances_(rtype::client::EntityAppearances::fromTexts({})),
      connection_(
          rtype::network::PERFECT_NETWORK,
          rtype::client::ContentPlaces{
              .assetFolder = rtype::engine::locateAssetFolder(),
              .cacheFolder = rtype::client::testing::unusedCacheFolder()}) {}

void PlayerConnectionFixture::provideTheParts() {
  rtype::engine::InstallContext context{std::string{PROVIDING_MODULE_NAME},
                                        services_, stages_};
  context.provide(appearances_);
  connection_.provideParts(context);
}

const rtype::engine::ServiceRegistry& PlayerConnectionFixture::services()
    const {
  return services_;
}
