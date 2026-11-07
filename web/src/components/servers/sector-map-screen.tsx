'use client';

import { useRouter } from 'next/navigation';
import type { Locale } from '@/i18n/locales';
import { pathOf, serverPathOf } from '@/lib/site-paths';
import { SectorMap } from './sector-map/sector-map';
import type { MapServer, SectorMapLabels } from './sector-map/types';

export type SectorMapScreenProps = {
  servers: readonly MapServer[];
  labels: SectorMapLabels;
  locale: Locale;
};

/**
 * The sector map as a page of the site: its Fiche, Classement and Heberger
 * buttons go to the pages of the language, and Escape on the whole sky goes
 * home. The map is read only, since a seat is taken from the game.
 */
export function SectorMapScreen({
  servers,
  labels,
  locale,
}: SectorMapScreenProps) {
  const router = useRouter();

  return (
    <SectorMap
      servers={servers}
      labels={labels}
      locale={locale}
      onOpenServer={id => router.push(serverPathOf(locale, id))}
      onLeaderboard={() => router.push(pathOf(locale, 'ranking'))}
      onHost={() => router.push(pathOf(locale, 'hostServer'))}
      onExit={() => router.push(pathOf(locale, 'home'))}
    />
  );
}
