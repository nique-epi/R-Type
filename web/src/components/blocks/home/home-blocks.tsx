import { StoryBand } from '@/components/home/story-band';
import {
  AutoRefresh,
  SERVERS_REFRESH_SECONDS,
} from '@/components/layout/auto-refresh';
import { Professor } from '@/components/sprites/professor';
import { getLatestRelease, getServerList } from '@/hooks/api';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { Bydos } from './bydos';
import { Explore } from './explore';
import { Features } from './features';
import { FreeSeats } from './free-seats';
import { Hangar } from './hangar';
import { Hero } from './hero';
import { News } from './news';
import { OpenSource } from './open-source';

/** The whole home page, one block after the other. */
export async function HomeBlocks({ lang }: { lang: Locale }) {
  const { story } = getDictionary(lang).pages.home;
  // The free seats read the list of servers: ask it now, so it is not waited for behind GitHub.
  void getServerList();
  const release = await getLatestRelease();

  return (
    <main>
      <AutoRefresh seconds={SERVERS_REFRESH_SECONDS} />
      <Hero lang={lang} version={release?.version} />
      <FreeSeats lang={lang} />
      <Bydos lang={lang} version={release?.version} />
      <Features lang={lang} />
      <Hangar lang={lang} />
      <StoryBand
        figure={<Professor scale={3} />}
        titleId="story-title"
        title={story.title}
        lede={story.lede}
      />
      <Explore lang={lang} />
      <News lang={lang} release={release} />
      <OpenSource lang={lang} />
    </main>
  );
}
