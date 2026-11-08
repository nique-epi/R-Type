import { ExploreTile, ExploreTiles } from '@/components/home/explore-tile';
import { SectionHeader } from '@/components/home/section-header';
import { Reveal } from '@/components/motion/reveal';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { pathOf, wikiPathOf } from '@/lib/site-paths';

/** The four tiles that lead to the rest of the site. */
export function Explore({ lang }: { lang: Locale }) {
  const { explore } = getDictionary(lang).pages.home;
  const hrefs = [
    wikiPathOf(lang, 'enemies'),
    wikiPathOf(lang, 'keys'),
    pathOf(lang, 'ranking'),
    pathOf(lang, 'servers'),
  ];

  return (
    <Section aria-labelledby="explore-title">
      <Container>
        <Reveal>
          <SectionHeader
            eyebrow={explore.eyebrow}
            title={explore.title}
            titleId="explore-title"
          />
        </Reveal>
        <Reveal targets='[data-slot="explore-tile"]'>
          <ExploreTiles>
            {explore.tiles.map((tile, index) => (
              <ExploreTile
                key={tile.title}
                href={hrefs[index] ?? pathOf(lang, 'home')}
                label={tile.label}
                title={tile.title}
                more={explore.more}
              />
            ))}
          </ExploreTiles>
        </Reveal>
      </Container>
    </Section>
  );
}
