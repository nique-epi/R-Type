import { Reveal } from '@/components/motion/reveal';
import { Heading, Lede } from '@/components/primitives/text/typography';
import { PaletteSection } from '@/components/wiki/palette-section';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { PALETTE_COLORS, PALETTE_IDS } from '@/lib/rtype/palettes';

/** The palettes of the pilots, for the readers who tell some colors apart badly. */
export function Accessibility({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const { accessibility } = dictionary.pages.wikiPage;

  return (
    <Reveal className="grid gap-8">
      <div>
        <Heading as="h2" size="article">
          {accessibility.title}
        </Heading>
        <Lede>{accessibility.lede}</Lede>
      </div>
      <PaletteSection
        chosen={accessibility.chosen}
        labels={dictionary.palette}
        palettes={PALETTE_IDS.map(id => ({
          id,
          label: accessibility.palettes[id],
          colors: PALETTE_COLORS[id],
        }))}
      />
    </Reveal>
  );
}
