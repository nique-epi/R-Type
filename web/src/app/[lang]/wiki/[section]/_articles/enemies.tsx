import { Reveal } from '@/components/motion/reveal';
import { Heading } from '@/components/primitives/text/typography';
import { Enemy } from '@/components/sprites/enemy';
import { ENEMY_KINDS } from '@/components/sprites/sheets';
import { BestiaryBook } from '@/components/wiki/bestiary-book';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The five Bydos of the game in the flight log, one page each. */
export function Enemies({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const { enemies } = dictionary.pages.wikiPage;

  return (
    <Reveal className="grid gap-8">
      <Heading as="h2" size="article">
        {enemies.title}
      </Heading>
      <BestiaryBook
        labels={dictionary.bestiary}
        entries={ENEMY_KINDS.map((kind, index) => {
          const entry = enemies.entries[kind];
          return {
            id: kind,
            name: entry.name,
            text: entry.text,
            caption: format(enemies.caption, {
              number: index + 1,
              name: entry.name,
            }),
            sprite: (
              <Enemy
                kind={kind}
                label={format(enemies.spriteAlt, { name: entry.name })}
              />
            ),
          };
        })}
      />
    </Reveal>
  );
}
