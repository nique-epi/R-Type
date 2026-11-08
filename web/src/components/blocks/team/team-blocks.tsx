import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { TeamCard } from '@/components/team/team-card';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { TEAM } from '@/lib/rtype/team';

/** The team page under its hero: the four members and their GitHub. */
export function TeamBlocks({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.teamPage;

  return (
    <>
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody targets='[data-slot="team-card"]' from={{ y: 64 }}>
        <div className="grid grid-cols-[repeat(auto-fit,minmax(220px,1fr))] gap-6">
          {TEAM.map(member => (
            <TeamCard
              key={member.handle}
              name={member.name}
              url={member.url}
              player={member.player}
              linkLabel={format(dictionary.team.github, {
                handle: member.handle,
              })}
            />
          ))}
        </div>
      </PageBody>
    </>
  );
}
