import { StoryBand } from '@/components/home/story-band';
import { Button } from '@/components/primitives/actions/button';
import { Ship } from '@/components/sprites/ship';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { TEAM } from '@/lib/rtype/team';

/** The band that says the code is open, with a link to the GitHub of each member of the team. */
export function OpenSource({ lang }: { lang: Locale }) {
  const { open } = getDictionary(lang).pages.home;

  return (
    <StoryBand
      align="start"
      titleId="open-title"
      title={open.title}
      lede={open.lede}
      className="mb-16"
    >
      {TEAM.map(member => (
        <Button
          key={member.handle}
          href={member.url}
          target="_blank"
          rel="noopener noreferrer"
        >
          <Ship player={member.player} scale={1} />
          {member.handle}
        </Button>
      ))}
    </StoryBand>
  );
}
