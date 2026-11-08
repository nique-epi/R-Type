import { HangarLocker } from '@/components/home/hangar-locker';
import { ItemPickup } from '@/components/home/item-pickup';
import { SectionHeader } from '@/components/home/section-header';
import { Reveal } from '@/components/motion/reveal';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { Lede } from '@/components/primitives/text/typography';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { withoutAccents } from '@/lib/rtype/pixel-text';
import { LOOK_ITEMS, lookItemSourceOf } from '@/lib/rtype/ship-looks';

/** The hangar: the items a ship wears, the ones won in a mission, and a pickup to show how. */
export function Hangar({ lang }: { lang: Locale }) {
  const { hangar } = getDictionary(lang).pages.home;

  return (
    <Section aria-labelledby="hangar-title">
      <Container className="grid gap-10">
        <Reveal className="grid gap-4">
          <SectionHeader
            eyebrow={hangar.eyebrow}
            title={hangar.title}
            titleId="hangar-title"
          />
          <Lede className="mt-0">{hangar.lede}</Lede>
        </Reveal>
        <Reveal className="grid gap-4">
          <ItemPickup
            item="crown"
            label={hangar.pickup.aria}
            won={withoutAccents(hangar.pickup.won)}
          />
          <p className="m-0 text-lg leading-[30px]">{hangar.pickup.text}</p>
        </Reveal>
        <Reveal>
          <HangarLocker
            aria-label={hangar.locker}
            cells={LOOK_ITEMS.map(item => {
              const source = lookItemSourceOf(item);
              return {
                item,
                source,
                name: withoutAccents(hangar.items[item]),
                sourceLabel: hangar.sources[source],
              };
            })}
          />
        </Reveal>
      </Container>
    </Section>
  );
}
