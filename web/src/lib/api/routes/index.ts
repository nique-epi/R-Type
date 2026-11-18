import 'server-only';

import * as leaderboard from './leaderboard';
import * as servers from './servers';
import * as sessions from './sessions';
import * as users from './users';

export const api = {
  leaderboard,
  servers,
  sessions,
  users,
};
