#pragma once

#include <functional>
#include "ReceivedDatagrams.hpp"

namespace rtype::server {

/**
 * @brief Called once per tick by the ServerApplication, on the simulation
 * thread, with the datagrams received since the previous tick. An exception it
 * throws stops the server and leaves ServerApplication::run().
 *
 * Anyone can send a datagram, so the handler must drop and count a datagram it
 * cannot decode, and never let an exception caused by a datagram's content
 * escape: one truncated datagram would otherwise stop the server for every
 * player.
 */
using TickHandler = std::function<void(const ReceivedDatagrams&)>;

}  // namespace rtype::server
