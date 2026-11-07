import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { RankingSection } from '@/components/ranking/ranking-section';
import { StarLayers } from '@/components/sky/star-layers';
import { getServerNames } from '@/hooks/api';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The leaderboard page under its hero: the screen of the game with example scores, on a sky of drifting stars that fills the whole page. */
export async function RankingBlocks({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.rankingPage;
  const servers = await getServerNames();

  return (
    <div className="relative isolate">
      <StarLayers className="-z-10" />
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody>
        <RankingSection
          locale={lang}
          servers={servers}
          copy={copy}
          labels={dictionary.ranking}
        />
      </PageBody>
    </div>
  );
}
