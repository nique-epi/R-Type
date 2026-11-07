import type { Metadata } from 'next';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { TeamCard } from '@/components/team/team-card';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';
import { TEAM } from '@/lib/rtype/team';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/team'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.teamPage.title };
}

export default async function TeamPage({ params }: PageProps<'/[lang]/team'>) {
  const lang = await readLocale(params);
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
