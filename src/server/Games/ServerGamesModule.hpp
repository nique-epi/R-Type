#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "EntitySheets.hpp"
#include "GameInstance.hpp"
#include "GameSettings.hpp"
#include "GamesSettings.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "IResultsArchive.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "RunningGame.hpp"
#include "SettingValue.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server {

/**
 * @brief The module that runs the game of the process, on a bus of its own.
 *
 * The game runs on the simulation thread. In the simulation stage, it reads
 * the datagrams of its own players (see keepDatagramsOf()), leaving out those
 * of any other sender the network admitted, such as the supervisor, and moves
 * on, and the players whose session closed are forgotten by the network. In
 * the outputs stage, its bus delivers the events of the tick. The lobby module
 * adds its work to the simulation stage after this one. The game reads real
 * time on the clock of the module for its Heartbeats.
 *
 * install() declares the `seed`, `state-rate`, `state-encoding` and
 * `input-delay` settings and provides RunningGame, and its bus as IGameEvents.
 * start() reads the entity sheets and the waves file, then creates the game
 * with the values of the settings, subscribing its rules and its broadcaster
 * to the bus; the same values decide whether the game is vanilla (see
 * vanillaBreaches()), the state encoding aside, which changes how the states
 * travel, not the game. A finished game waits for players again, so the game
 * is never replaced. stop() tells every player the server shuts down (see
 * FarewellSender).
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
   * game. With a seed, the game starts from it, so it can be played again;
   * without one, it draws its own. The journal says the folder of the sheets
   * and their count, the difficulty the game plays and whether it is vanilla,
   * then one line per reason when it is not. The sheets are kept until the
   * module is destroyed, after its game.
   * @throws ServerException, game::GameException or engine::EngineException
   * (or a class derived from one) naming the file, the line and the key when a
   * sheet or the waves file cannot be read or does not describe what the
   * server expects, or when the waves file has no DEFAULT_DIFFICULTY.
   */
  void start() override;

  /** @brief Says goodbye to the players, then drops the broadcaster. */
  void stop() override;

 private:
  [[nodiscard]] std::unique_ptr<GameInstance> createGame(GameSettings settings);
  void declareSettings(engine::ConfigurationRegistry& configuration);
  void playGame();
  /** @returns The settings of the game, from the waves of @p catalog at
   * DEFAULT_DIFFICULTY and the sheets @p types, which must outlive the game,
   * starting from @p seed, or from a seed drawn at random without one; logs
   * how the game plays, judged with @p seed. */
  [[nodiscard]] GameSettings settingsOf(const WaveCatalog& catalog,
                                        const game::EntitySheets& types,
                                        std::optional<std::uint32_t> seed);

  GamesSettings settings_;
  const engine::SettingValue* seedSetting_{nullptr};
  const engine::SettingValue* stateRateSetting_{nullptr};
  const engine::SettingValue* stateEncodingSetting_{nullptr};
  const engine::SettingValue* inputDelaySetting_{nullptr};
  logging::Logger logger_;
  engine::SystemClock clock_;
  std::optional<game::EntitySheets> types_;
  RunningGame runningGame_;
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  IResultsArchive* results_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  std::vector<network::IncomingDatagram> datagramsOfGame_;
};

}  // namespace rtype::server
