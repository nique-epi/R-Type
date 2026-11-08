import type { PlayerNumber } from './players';

/** A member of the team, flying the ship of the seat they hold on the site. */
export type TeamMember = {
  name: string;
  handle: string;
  url: string;
  player: PlayerNumber;
};

export const TEAM: readonly TeamMember[] = [
  {
    name: 'Loann Bideau',
    handle: '@LoannBi',
    url: 'https://github.com/LoannBi',
    player: 1,
  },
  {
    name: 'Nolan Fribault',
    handle: '@nolanfrb',
    url: 'https://github.com/nolanfrb',
    player: 2,
  },
  {
    name: 'Yannis Lorang',
    handle: '@Yannislrg',
    url: 'https://github.com/Yannislrg',
    player: 3,
  },
  {
    name: 'Lukas Renaud',
    handle: '@0rezh',
    url: 'https://github.com/0rezh',
    player: 4,
  },
];
