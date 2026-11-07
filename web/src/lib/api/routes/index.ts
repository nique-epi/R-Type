import 'server-only';

import * as health from './health';
import * as servers from './servers';

export const api = {
  health,
  servers,
};
