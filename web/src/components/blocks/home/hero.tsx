import { GalleryStrip } from '@/components/home/gallery-strip';
import { HomeHero } from '@/components/home/home-hero';
import { Button } from '@/components/primitives/actions/button';
import { Triangle } from '@/components/primitives/icons/triangle';
import { Enemy } from '@/components/sprites/enemy';
import { Professor } from '@/components/sprites/professor';
import { Ship } from '@/components/sprites/ship';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { pathOf } from '@/lib/site-paths';

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

/** The hero of the home page, and the strip of sprites that slides under it. */
export function Hero({
  lang,
  version,
}: {
  lang: Locale;
  /** The version of the latest release; nothing when it could not be read. */
  version: string | undefined;
}) {
  const { pages, header, hero } = getDictionary(lang);
  const copy = pages.home;

  return (
    <>
      <HomeHero
        aria-label="R-Type"
        eyebrow={
          version ? format(copy.eyebrow, { version }) : copy.eyebrowFallback
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
    </>
  );
}
