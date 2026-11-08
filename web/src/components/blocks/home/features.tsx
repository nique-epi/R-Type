import { FeatureRow } from '@/components/home/feature-row';
import { Reveal } from '@/components/motion/reveal';
import { ArrowLink } from '@/components/primitives/actions/arrow-link';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { Enemy } from '@/components/sprites/enemy';
import { Ship } from '@/components/sprites/ship';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { pathOf, wikiPathOf } from '@/lib/site-paths';

/** The three features of the game, each on a fill of its own, the art on alternate sides. */
export function Features({ lang }: { lang: Locale }) {
  const { features } = getDictionary(lang).pages.home;
  const [multiplayer, hosting, accessible] = features;
  const rows = [
    {
      copy: multiplayer,
      href: pathOf(lang, 'servers'),
      fill: 'bg-planet-2',
      art: <Ship player={2} scale={8} />,
    },
    {
      copy: hosting,
      href: wikiPathOf(lang, 'host'),
      fill: 'bg-player-dark-blue',
      art: <Enemy kind="bydo" scale={6} />,
    },
    {
      copy: accessible,
      href: wikiPathOf(lang, 'accessibility'),
      fill: 'bg-deep-blue',
      art: <Ship player={3} scale={8} />,
    },
  ];

  return (
    <Section aria-label="R-Type">
      <Container>
        <Reveal start="top 88%" duration={1}>
          {rows.map(({ copy, href, fill, art }, index) =>
            copy ? (
              <FeatureRow
                key={copy.title}
                reversed={index % 2 === 1}
                artClassName={fill}
                art={art}
                label={copy.label}
                title={copy.title}
                text={copy.text}
                action={<ArrowLink href={href}>{copy.cta}</ArrowLink>}
              />
            ) : null,
          )}
        </Reveal>
      </Container>
    </Section>
  );
}
