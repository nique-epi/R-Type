#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <type_traits>
#include <utility>
#include <variant>
#include "ActionSet.hpp"
#include "AnimationSystem.hpp"
#include "ArrivalReveal.hpp"
#include "BossBar.hpp"
#include "BossBarConstants.hpp"
#include "BossLook.hpp"
#include "BossPresentation.hpp"
#include "BossStatus.hpp"
#include "ChatWindow.hpp"
#include "DepartureFeed.hpp"
#include "EffectPlayed.hpp"
#include "EffectPlayer.hpp"
#include "EffectPreferences.hpp"
#include "EffectSpawner.hpp"
#include "EnemyAction.hpp"
#include "EnemyActionPlayer.hpp"
#include "EntityFeedback.hpp"
#include "EntityMessage.hpp"
#include "EntityReplicator.hpp"
#include "EntityReplicatorParts.hpp"
#include "FinalRanking.hpp"
#include "FlashPreference.hpp"
#include "GameEnded.hpp"
#include "GameNoticeConstants.hpp"
#include "GameResult.hpp"
#include "GameScreenParts.hpp"
#include "GroundEffectSystem.hpp"
#include "HitFlashSystem.hpp"
#include "IEntityLayer.hpp"
#include "IEntityMessageSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreenNavigator.hpp"
#include "ISky.hpp"
#include "InterpolationClock.hpp"
#include "InterpolationTally.hpp"
#include "LevelStatus.hpp"
#include "Logger.hpp"
#include "MissionBanner.hpp"
#include "MissionStart.hpp"
#include "OwnMissilesParts.hpp"
#include "OwnPrediction.hpp"
#include "OwnShip.hpp"
#include "PilotNameTags.hpp"
#include "PilotShipLooks.hpp"
#include "PlayerAction.hpp"
#include "PlayerCommand.hpp"
#include "PlayerLeft.hpp"
#include "PlayerPalette.hpp"
#include "PlayerPreferences.hpp"
#include "PositionInterpolation.hpp"
#include "ScoreBoard.hpp"
#include "ShipFlightSystem.hpp"
#include "SkyLevel.hpp"
// NOLINTNEXTLINE(misc-include-cleaner)
#include "GameScreenIdConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayerSettings.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SpriteEffectSystem.hpp"
#include "StatusBand.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @brief Gives @p tick to the clock that shows the other entities and to
 * @p tally, which also counts how far the state moved the tick shown. */
void observeShownTick(InterpolationClock& clock, InterpolationTally& tally,
                      std::uint32_t tick) {
  const std::optional<double> before = clock.renderTick();
  clock.observe(tick);
  tally.observe(tick);
  const std::optional<double> after = clock.renderTick();
  if (before.has_value() && after.has_value() && *after != *before) {
    tally.countRecalibration(std::abs(*after - *before));
  }
}

/** @returns How many pixels of the canvas the sky and the entities are drawn
 * away from their place: the camera shake, (0, 0) at rest. */
sf::Vector2i cameraOffsetOf(EffectSpawner& effects) {
  return effects.tools().camera.get().offset();
}

/** @brief Sets the speed of the stars and the shake of the sky for the time
 * since the start of the game: the warp of MISSION START with flashes, their
 * own speed without. */
void moveSky(ISky& sky, EffectSpawner& effects, FlashPreference preference) {
  sky.setSpeedFactor(preference == FlashPreference::Full
                         ? warpSpeedFactorAt(effects.sinceStart())
                         : 1.0F);
  sky.setOffset(cameraOffsetOf(effects));
}

/** @brief Writes the actions @p held at the trace level when they are not the
 * ones written last, kept in @p traced. */
void traceHeldActions(const logging::Logger& logger, const ActionSet& held,
                      std::uint8_t& traced) {
  if (held.bits() == traced) {
    return;
  }
  logger.trace("actions held: ", static_cast<unsigned int>(held.bits()));
  traced = held.bits();
}

/** @brief Who the entity messages of the server go to, but the end of the
 * game and the world states, which the screen follows itself. */
struct EntityMessageRoutes {
  std::reference_wrapper<EntityReplicator> replicator;
  std::reference_wrapper<EnemyActionPlayer> enemyActions;
  std::reference_wrapper<EffectPlayer> namedEffects;
  std::reference_wrapper<BossPresentation> boss;
  std::reference_wrapper<ScoreBoard> scoreBoard;
  std::reference_wrapper<DepartureFeed> departures;
  std::reference_wrapper<ISky> sky;
};

/** @returns What the sky shows of the level @p status tells. */
SkyLevel skyLevelOf(const network::LevelStatus& status) {
  return SkyLevel{.biome = status.biome,
                  .scrollPosition = status.scrollPosition,
                  .scrollSpeed = status.scrollSpeed};
}

/** @brief Hands an entity message to whoever it goes to: a PlayerLeft to the
 * board and the feed, an EnemyAction to the enemy it animates and to the boss,
 * an EffectPlayed to the player of the named effects, a BossStatus to the
 * boss, a LevelStatus to the sky, anything else but GameEnded to the
 * replicator. */
void routeEntityMessage(const EntityMessageRoutes& routes,
                        const EntityMessage& message) {
  std::visit(
      [&routes](const auto& announced) {
        using Announced = std::decay_t<decltype(announced)>;
        if constexpr (std::is_same_v<Announced, network::PlayerLeft>) {
          routes.scoreBoard.get().apply(announced);
          routes.departures.get().follow(routes.scoreBoard.get().pilots());
        } else if constexpr (std::is_same_v<Announced, network::EnemyAction>) {
          routes.enemyActions.get().apply(announced);
          routes.boss.get().apply(announced);
        } else if constexpr (std::is_same_v<Announced, network::EffectPlayed>) {
          routes.namedEffects.get().apply(announced);
        } else if constexpr (std::is_same_v<Announced, network::BossStatus>) {
          routes.boss.get().apply(announced);
        } else if constexpr (std::is_same_v<Announced, network::LevelStatus>) {
          routes.sky.get().showLevel(skyLevelOf(announced));
        } else if constexpr (!std::is_same_v<Announced, network::GameEnded>) {
          routes.replicator.get().apply(announced);
        }
      },
      message);
}

/** @brief Moves the effects on by @p sinceTheWorld, with the flashes, the
 * shades and the sound cues @p settings set. */
void advanceEffects(EffectSpawner& effects, const PlayerSettings& settings,
                    engine::Duration sinceTheWorld) {
  effects.advance(sinceTheWorld,
                  EffectPreferences{.flashes = flashPreferenceOf(settings),
                                    .shades = pilotShadesOf(settings),
                                    .soundCues = settings.soundCues});
}

/** @brief Puts the other entities at the tick @p clock shows, then reveals
 * those that arrived (see placeInterpolatedPositions() and
 * revealArrivedEntities()). */
void showOtherEntities(const InterpolationClock& clock,
                       engine::ComponentRegistry& components,
                       const EntityFeedback& feedback) {
  placeInterpolatedPositions(clock, components);
  revealArrivedEntities(clock, components, feedback);
}

}  // namespace

GameScreen::GameScreen(GameScreenParts parts,
                       std::unique_ptr<IEntityLayer> entityLayer,
                       std::unique_ptr<IInterfaceCanvas> statusCanvas)
    : navigator_(&parts.navigator.get()),
      entityLayer_(std::move(entityLayer)),
      statusCanvas_(std::move(statusCanvas)),
      settings_(&parts.settings.get()),
      effects_(entities_, components_, std::random_device{}()),
      feedback_(EntityFeedback{.effects = effects_, .sounds = parts.sounds}),
      prediction_(parts.connection.get().status().playerNumber,
                  OwnMissilesParts{.entities = entities_,
                                   .components = components_,
                                   .feedback = feedback_,
                                   .types = parts.entityTypes}),
      replicator_(EntityReplicatorParts{
          .entities = entities_,
          .components = components_,
          .feedback = feedback_,
          .types = parts.entityTypes,
          .ownPlayerNumber = parts.connection.get().status().playerNumber,
          .ownMissiles = &prediction_.missiles()}),
      enemyActions_(components_),
      namedEffects_(effects_.tools()),
      boss_(effects_.tools(), parts.sounds, parts.audio),
      playerCommands_(true, parts.settings.get().keys),
      inputs_(&parts.inputs.get()),
      entityMessageSource_(&parts.entityMessages.get()),
      serverTicks_(&parts.serverTicks.get()),
      shipLooks_(&parts.shipLooks.get()),
      scoreBoard_(parts.waitingRoom.get().lobbyPlayers(),
                  parts.connection.get().status().playerNumber),
      result_(&parts.result.get()),
      playerNumber_(parts.connection.get().status().playerNumber),
      interpolationTally_(&parts.interpolationTally.get()),
      sky_(&parts.sky.get()),
      chat_(parts.chat.get(), parts.settings.get()) {
  interpolationTally_->startGame();
  systems_.add(std::make_unique<AnimationSystem>());
  systems_.add(std::make_unique<HitFlashSystem>());
  systems_.add(std::make_unique<SpriteEffectSystem>());
  systems_.add(std::make_unique<GroundEffectSystem>());
  systems_.add(std::make_unique<ShipFlightSystem>());
  moveSky(*sky_, effects_, flashPreferenceOf(*settings_));
  entityLayer_->paint(components_, cameraOffsetOf(effects_));
  repaintStatus(boss_.look(flashPreferenceOf(*settings_)));
}

GameScreen::~GameScreen() {
  interpolationTally_->endGame();
  sky_->setSpeedFactor(1.0F);
  sky_->setOffset({});
  sky_->showLevel(SkyLevel{});
}

void GameScreen::handleEvent(const sf::Event& event) {
  if (chat_.handleEvent(event)) {
    playerCommands_.releaseAll();
    return;
  }
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    playerCommands_.releaseAll();
    navigator_->push(GAME_OPTIONS_SCREEN);
  }
  if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
    playerCommands_.keyPressed(keyPressed->scancode);
  } else if (released != nullptr) {
    playerCommands_.keyReleased(released->scancode);
  } else if (event.is<sf::Event::FocusLost>()) {
    playerCommands_.focusLost();
  } else if (event.is<sf::Event::FocusGained>()) {
    playerCommands_.focusGained();
  }
}

void GameScreen::update(engine::Duration elapsed) {
  const PlayerCommand command{.held = playerActions(),
                              .shownTick = interpolationClock_.shownTick()};
  prediction_.advance(elapsed, command, *inputs_);
  traceHeldActions(logger_, command.held, loggedActions_);
  systems_.run(components_, elapsed);
  const std::optional<double> serverTick = serverTicks_->serverTickNow();
  interpolationClock_.follow(serverTick);
  interpolationClock_.advance(elapsed);
  const engine::Duration sinceTheWorld =
      worldSeen_ ? elapsed : engine::Duration::zero();
  advanceEffects(effects_, *settings_, sinceTheWorld);
  departures_.advance(elapsed);
  silence_.advance(sinceTheWorld);
  boss_.advance(elapsed);
  applyEntityMessages();
  showOtherEntities(interpolationClock_, components_, feedback_);
  interpolationTally_->countFrame(interpolationClock_.renderTick(), serverTick);
  prediction_.place(replicator_, components_);
  tagPilotShips(components_, scoreBoard_.pilots(), *settings_);
  dressPilotShips(components_, shipLooks_->playerLooks());
  askForTheEndWhenDue();
  moveSky(*sky_, effects_, flashPreferenceOf(*settings_));
  entityLayer_->paint(components_, cameraOffsetOf(effects_));
  repaintStatusWhenChanged();
}

void GameScreen::applyEntityMessages() {
  entityMessageSource_->drainEntityMessages(receivedEntityMessages_);
  if (!receivedEntityMessages_.empty()) {
    silence_.hear();
  }
  const EntityMessageRoutes routes{.replicator = replicator_,
                                   .enemyActions = enemyActions_,
                                   .namedEffects = namedEffects_,
                                   .boss = boss_,
                                   .scoreBoard = scoreBoard_,
                                   .departures = departures_,
                                   .sky = *sky_};
  for (const EntityMessage& message : receivedEntityMessages_) {
    routeEntityMessage(routes, message);
    if (const auto* ended = std::get_if<network::GameEnded>(&message)) {
      recordEnd(*ended);
    }
    if (const auto* state = std::get_if<network::WorldState>(&message)) {
      applyWorldState(*state);
    }
  }
  receivedEntityMessages_.clear();
}

void GameScreen::applyWorldState(const network::WorldState& state) {
  worldSeen_ = true;
  scoreBoard_.apply(state);
  observeShownTick(interpolationClock_, *interpolationTally_, state.tick);
  if (const auto own = ownShipOf(state, playerNumber_)) {
    inputs_->confirmInputsUpTo(own->acknowledgedInput);
  }
  prediction_.apply(state);
}

void GameScreen::recordEnd(const network::GameEnded& ended) {
  *result_ = GameResult{.pilots = rankedPilots(flyingTheirHulls(
                            finalPilotsOf(scoreBoard_.pilots(), ended),
                            shipLooks_->playerLooks())),
                        .outcome = ended.outcome};
  worldSeen_ = true;
  endedAt_ = effects_.sinceStart();
  interpolationTally_->endGame();
  prediction_.ceaseFire();
  silence_.stop();
  boss_.endGame();
}

void GameScreen::askForTheEndWhenDue() {
  if (!endedAt_.has_value() ||
      effects_.sinceStart() - *endedAt_ < END_SCREEN_DELAY) {
    return;
  }
  endedAt_.reset();
  navigator_->replaceAll(END_SCREEN);
}

void GameScreen::repaintStatusWhenChanged() {
  const BossLook boss = boss_.look(flashPreferenceOf(*settings_));
  if (scoreBoard_.revision() != paintedStatusRevision_ ||
      missionBannerAt(effects_.sinceStart()) != paintedBanner_ ||
      paintedPalette_ != playerPaletteOf(*settings_) ||
      departures_.revision() != paintedDepartures_ ||
      silence_.isShown() != paintedSilence_ || boss != paintedBoss_ ||
      chat_.isShown() || paintedChat_) {
    repaintStatus(boss);
  }
}

void GameScreen::repaintStatus(const BossLook& boss) {
  const MissionBannerLook banner = missionBannerAt(effects_.sinceStart());
  const PlayerPalette& palette = playerPaletteOf(*settings_);
  statusCanvas_->clear();
  paintBossAlert(*statusCanvas_, boss);
  paintStatusBand(*statusCanvas_, scoreBoard_.pilots(), palette);
  paintMissionBanner(*statusCanvas_, banner);
  paintBossBar(*statusCanvas_, boss.bar);
  departures_.paint(
      *statusCanvas_, palette,
      boss.bar.shown ? DEPARTURE_FEED_UNDER_BOSS_BAR : DEPARTURE_FEED_TOP_LEFT);
  silence_.paint(*statusCanvas_);
  chat_.paint(*statusCanvas_, palette, GAME_CHAT_BOTTOM);
  statusCanvas_->display();
  paintedStatusRevision_ = scoreBoard_.revision();
  paintedBanner_ = banner;
  paintedPalette_ = palette;
  paintedDepartures_ = departures_.revision();
  paintedSilence_ = silence_.isShown();
  paintedChat_ = chat_.isShown();
  paintedBoss_ = boss;
}

ActionSet GameScreen::playerActions() const {
  ActionSet held = playerCommands_.held();
  if (settings_->autoFire) {
    held.add(PlayerAction::Shoot);
  }
  return held;
}

void GameScreen::draw(sf::RenderTarget& target) const {
  entityLayer_->show(target);
  statusCanvas_->show(target);
}

bool GameScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
