import type { DownloadPanelLabels } from '@/components/download/download-panel';
import type { SiteHeaderLabels } from '@/components/layout/site-header';
import type { RankingScreenLabels } from '@/components/ranking/ranking-screen';
import type { GamesTableLabels } from '@/components/servers/games-table';
import type { SectorMapLabels } from '@/components/servers/sector-map/types';
import type { BestiaryBookLabels } from '@/components/wiki/bestiary-book';
import type { PalettePickerProps } from '@/components/wiki/palette-picker';
import type { GameState, ServerStatus } from '@/lib/rtype/servers';
import type { Locale } from './locales';
import type { PageCopy } from './page-copy';

/**
 * The fixed words of the components, in one language. Each component names
 * the words it needs in its own label type; a dictionary gives them all.
 * The words of the content (a server, a release note) come from the data.
 */
export type Dictionary = {
  locale: Locale;
  status: Record<ServerStatus, string>;
  gameState: Record<GameState, string>;
  header: SiteHeaderLabels;
  download: DownloadPanelLabels;
  gamesTable: GamesTableLabels;
  chart: { now: string; period: string };
  sectorMap: SectorMapLabels;
  ranking: RankingScreenLabels;
  bestiary: BestiaryBookLabels;
  palette: PalettePickerProps['labels'];
  keyBindings: { key: string; action: string };
  wiki: { navigation: string };
  team: { github: string };
  pages: PageCopy;
};
