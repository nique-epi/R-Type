import {
  type ShowcaseVideo,
  VideoShowcase,
} from '@/components/home/video-showcase';
import { Reveal } from '@/components/motion/reveal';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { Eyebrow, Heading } from '@/components/primitives/text/typography';
import { Enemy } from '@/components/sprites/enemy';
import { Professor } from '@/components/sprites/professor';
import { Ship } from '@/components/sprites/ship';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The sky a scene of sprites is laid on. */
const SCENE = 'starfield absolute inset-0 block';

/** The picture of the trailer: a squadron facing a Bydo, a shot between them. */
function trailerScene() {
  return (
    <span className={SCENE}>
      <Ship player={1} scale={8} className="absolute top-[26%] left-[12%]" />
      <Ship player={2} scale={4} className="absolute top-[58%] left-[6%]" />
      <Enemy kind="bydo" scale={6} className="absolute top-[22%] right-[16%]" />
      <Enemy
        kind="drillback"
        scale={9}
        className="absolute top-[52%] right-[30%]"
      />
      <span className="absolute top-[36%] left-[34%] h-3 w-24 rounded-md bg-hud-cyan" />
    </span>
  );
}

/** A card of the row under the trailer: one sprite on a plain fill. */
function cardScene(fill: string, sprite: React.ReactNode) {
  return (
    <span className={`absolute inset-0 block ${fill}`}>
      <span className="absolute top-1/2 left-1/2 -translate-1/2">{sprite}</span>
    </span>
  );
}

/** The trailer and the three videos under it. */
export function Videos({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const { trailer } = dictionary.pages.home;
  const scenes = [
    cardScene('bg-player-dark-blue', <Ship player={3} scale={4} />),
    cardScene('bg-planet-2', <Enemy kind="bydomancer" scale={4} />),
    cardScene('bg-deep-blue', <Professor scale={3} />),
  ];
  const videos: ShowcaseVideo[] = trailer.videos.map((video, index) => ({
    id: `video-${index}`,
    ...video,
    scene: scenes[index],
  }));

  return (
    <Section aria-labelledby="videos-title" className="pt-0">
      <Container>
        <Reveal>
          <div className="mb-8">
            <Eyebrow>{trailer.eyebrow}</Eyebrow>
            <Heading id="videos-title" size="title">
              {trailer.title}
            </Heading>
          </div>
        </Reveal>
        <VideoShowcase
          trailer={{
            id: 'trailer',
            label: trailer.eyebrow,
            title: trailer.title,
            aria: trailer.watchAria,
            playLabel: dictionary.video.watch,
            scene: trailerScene(),
          }}
          videos={videos}
          figure={<Ship player={1} scale={4} />}
          comingSoon={dictionary.video.comingSoon}
          closeLabel={dictionary.video.close}
        />
      </Container>
    </Section>
  );
}
