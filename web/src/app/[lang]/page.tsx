import { connection } from 'next/server';
import { ExploreTile, ExploreTiles } from '@/components/home/explore-tile';
import { GalleryStrip } from '@/components/home/gallery-strip';
import { HomeHero } from '@/components/home/home-hero';
import { SectionHeader } from '@/components/home/section-header';
import { StoryBand } from '@/components/home/story-band';
import { Reveal } from '@/components/motion/reveal';
import { Button } from '@/components/primitives/actions/button';
import { Triangle } from '@/components/primitives/icons/triangle';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { Enemy } from '@/components/sprites/enemy';
import { Professor } from '@/components/sprites/professor';
import { Ship } from '@/components/sprites/ship';
import { getLatestRelease } from '@/hooks/api';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';
import { pathOf, wikiPathOf } from '@/lib/site-paths';
import { Bydos } from './_home/bydos';
import { Features } from './_home/features';
import { FreeSeats } from './_home/free-seats';
import { News } from './_home/news';
import { OpenSource } from './_home/open-source';
import { Videos } from './_home/videos';

/** The sprites of the strip under the hero: the four ships and the five Bydos, the professor among them. */
const STRIP = [
  <Ship key="p1" player={1} scale={4} />,
  <Enemy key="bydo" kind="bydo" scale={4} />,
  <Ship key="p2" player={2} scale={4} />,
  <Enemy key="voltjelly" kind="voltjelly" scale={3} />,
  <Ship key="p3" player={3} scale={4} />,
  <Enemy key="drillback" kind="drillback" scale={4} />,
  <Ship key="p4" player={4} scale={4} />,
  <Enemy key="reaper" kind="reaper" scale={4} />,
  <Professor key="professor" scale={2} />,
  <Enemy key="bydomancer" kind="bydomancer" scale={4} />,
];

export default async function HomePage({ params }: PageProps<'/[lang]'>) {
  await connection();
  const lang = await readLocale(params);
  const { pages, header, hero } = getDictionary(lang);
  const copy = pages.home;
  const release = await getLatestRelease();
  const tileHrefs = [
    wikiPathOf(lang, 'enemies'),
    wikiPathOf(lang, 'keys'),
    pathOf(lang, 'ranking'),
    pathOf(lang, 'servers'),
  ];
  const tiles = copy.explore.tiles.map((tile, index) => ({
    ...tile,
    href: tileHrefs[index] ?? pathOf(lang, 'home'),
  }));

  return (
    <main>
      <HomeHero
        aria-label="R-Type"
        eyebrow={
          release
            ? format(copy.eyebrow, { version: release.version })
            : copy.eyebrowFallback
        }
        logoAlt={header.logo}
        actions={
          <>
            <Button href={pathOf(lang, 'servers')} variant="solid">
              <Triangle />
              {copy.play}
            </Button>
            <Button href={pathOf(lang, 'download')}>{copy.download}</Button>
          </>
        }
        scrollCue={hero.scrollCue}
        tagline={copy.taglineLines.flatMap((line, index) =>
          index === 0 ? [line] : [<br key={line} />, line],
        )}
        facts={
          <>
            <span>
              <b className="font-normal text-star-white">{copy.multiplayer}</b>{' '}
              {copy.upToFour}
            </span>
            <span>Linux</span>
            <span>Windows</span>
            <span>macOS</span>
          </>
        }
      />
      <GalleryStrip aria-label={copy.gallery} tiles={STRIP} />
      <FreeSeats lang={lang} />
      <Videos lang={lang} />
      <Bydos lang={lang} version={release?.version} />
      <Features lang={lang} />
      <StoryBand
        figure={<Professor scale={3} />}
        titleId="story-title"
        title={copy.story.title}
        lede={copy.story.lede}
      />
      <Section aria-labelledby="explore-title">
        <Container>
          <Reveal>
            <SectionHeader
              eyebrow={copy.explore.eyebrow}
              title={copy.explore.title}
              titleId="explore-title"
            />
          </Reveal>
          <Reveal targets='[data-slot="explore-tile"]'>
            <ExploreTiles>
              {tiles.map(tile => (
                <ExploreTile
                  key={tile.href}
                  href={tile.href}
                  label={tile.label}
                  title={tile.title}
                  more={copy.explore.more}
                />
              ))}
            </ExploreTiles>
          </Reveal>
        </Container>
      </Section>
      <News lang={lang} release={release} />
      <OpenSource lang={lang} />
    </main>
  );
}
