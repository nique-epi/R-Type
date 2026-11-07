#pragma once

#include <vector>
#include "ExitedGame.hpp"
#include "GameRegistry.hpp"
#include "IDatagramSender.hpp"
#include "StopPause.hpp"

namespace rtype::server {

/**
 * @brief Stops every game of a registry the way the supervisor stops itself:
 * it tells each game to stop (a Disconnection under its link token, sent
 * FAREWELL_COPIES times, FAREWELL_COPY_INTERVAL apart, since nothing repeats
 * it), waits up to STOP_GRACE_WAIT for the processes to end, then kills the
 * ones that remain.
 */
class ChildrenStopper {
 public:
  /** @param sender Where the orders go; it must outlive the stopper.
   * @param pause How the stopper lets time pass between two looks. */
  ChildrenStopper(IDatagramSender& sender, StopPause pause);

  /**
   * @brief Stops every game of @p registry, which is empty when it returns.
   * @returns Every game that ended meanwhile, with how, in the order the
   * stopper saw them end.
   */
  [[nodiscard]] std::vector<ExitedGame> stopAll(GameRegistry& registry);

 private:
  void queueStopOrders(const GameRegistry& registry);

  IDatagramSender* sender_;
  StopPause pause_;
};

}  // namespace rtype::server
