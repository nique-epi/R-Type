import 'server-only';

import * as health from './health';
import * as leaderboard from './leaderboard';
import * as servers from './servers';

export const api = {
  health,
  leaderboard,
  servers,
};
