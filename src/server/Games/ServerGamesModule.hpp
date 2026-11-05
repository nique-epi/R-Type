#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameSettings.hpp"
#include "Games.hpp"
#include "GamesSettings.hpp"
#include "HostedGamesWatch.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "IResultsArchive.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "LatestHostedGames.hpp"
#include "Logger.hpp"
#include "SettingValue.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server {

/**
 * @brief The module that runs the games: GamesSettings::gameCount games, each
 * with its own bus.
 *
 * Every game runs on the simulation thread, one after the other in each tick:
 * no state is shared between games, so nothing needs a lock, but an exception
 * in one game stops the server and a slow game slows the others. In the
 * simulation stage, each game reads the datagrams of its own players (see
 * keepDatagramsOf()) and moves on, and the players whose session closed are
 * forgotten by the network. In the outputs stage, the bus of each game delivers
 * the events of the tick. The lobby module adds its
 * work to the simulation stage after this one. Each game reads real time on the
 * clock of the module for its Heartbeats.
 *
 * install() builds the empty list of games, declares the `seed`,
 * `state-rate` and `input-delay` settings and provides Games. start() reads the
 * entity sheets and the waves file once, then creates the games with the values
 * of the settings, each subscribing its rules and its broadcaster to its own
 * bus; the same values decide whether the games are vanilla (see
 * vanillaBreaches()). A finished game waits for players again, so the games are
 * never replaced. stop() tells every player the server shuts down (see
 * FarewellSender). The module provides the list of its games as
 * LatestHostedGames, published again at the end of each tick in which a player
 * arrived or left or a game changed state (see HostedGamesWatch).
 */
class ServerGamesModule final : public engine::IModule {
 public:
  explicit ServerGamesModule(GamesSettings settings);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when the network module did not provide
   * the sender, the admission or the datagrams of the tick, or the storage
   * module the results archive.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Reads the entity sheets, then the waves file, and creates the
   * games. With a seed, game n (from 0) starts from the seed plus n times
   * GAME_SEED_STRIDE, so each game can be played again; without one, each game
   * draws its own. The journal says the folder of the sheets and their count,
   * the difficulty the games play and whether they are vanilla, then one line
   * per reason when they are not. The sheets are kept until the module is
   * destroyed, after its games.
   * @throws NoJoinableGameException when there is no game to create, and
   * TooManyGamesException when there are more than a heartbeat lists.
   * @throws ServerException, game::GameException or engine::EngineException
   * (or a class derived from one) naming the file, the line and the key when a
   * sheet or the waves file cannot be read or does not describe what the
   * server expects, or when the waves file has no DEFAULT_DIFFICULTY.
   */
  void start() override;

  /** @brief Says goodbye to the players, then drops the broadcaster. */
  void stop() override;

 private:
  [[nodiscard]] std::unique_ptr<GameInstance> createGame(
      engine::EventBus& events, GameSettings settings);
  void declareSettings(engine::ConfigurationRegistry& configuration);
  void playGames();
  /** @returns The settings every game shares, its seed aside, from the waves
   * of @p catalog at DEFAULT_DIFFICULTY and the sheets @p types, which must
   * outlive the games; logs how the games play, judged with @p seed. */
  [[nodiscard]] GameSettings sharedSettingsOf(
      const WaveCatalog& catalog, const game::EntitySheets& types,
      std::optional<std::uint32_t> seed);
  /** @brief Creates GamesSettings::gameCount games from @p shared, each with
   * a seed of its own drawn from @p seed, or at random without one. */
  void createGames(const GameSettings& shared,
                   std::optional<std::uint32_t> seed);

  GamesSettings settings_;
  const engine::SettingValue* seedSetting_{nullptr};
  const engine::SettingValue* stateRateSetting_{nullptr};
  const engine::SettingValue* inputDelaySetting_{nullptr};
  logging::Logger logger_;
  engine::SystemClock clock_;
  std::optional<game::EntitySheets> types_;
  Games games_;
  LatestHostedGames hostedGames_;
  std::optional<HostedGamesWatch> watch_;
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  IResultsArchive* results_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  std::vector<network::IncomingDatagram> datagramsOfGame_;
};

}  // namespace rtype::server
