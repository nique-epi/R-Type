#pragma once

namespace rtype::client {

/**
 * @brief Runs what Velopack does at the start of an installed client, before
 * anything else: the installer and the updater start the client with
 * arguments of their own (--veloapp-install, --veloapp-updated, ...), for
 * which Velopack runs its hooks and ends the process; an update downloaded
 * but not applied is applied, the client then started again. Outside an
 * install, as from a build folder, it logs and returns.
 */
void runVelopackStartup();

}  // namespace rtype::client
