import { BydoParade } from '@/components/home/bydo-parade';
import { ArrowLink } from '@/components/primitives/actions/arrow-link';
import { Enemy } from '@/components/sprites/enemy';
import { ENEMY_KINDS, enemyScaleFor } from '@/components/sprites/sheets';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { wikiPathOf } from '@/lib/site-paths';

/** How wide each Bydo is drawn on its panel, in pixels. */
const PANEL_SPRITE_PIXELS = 216;

/** The five Bydos of the game, one panel each, that slide sideways as the page scrolls. */
export function Bydos({
  lang,
  version,
}: {
  lang: Locale;
  version: string | undefined;
}) {
  const { pages } = getDictionary(lang);
  const { bydos } = pages.home;

  return (
    <BydoParade
      aria-label={bydos.aria}
      eyebrow={bydos.eyebrow}
      title={bydos.title}
      lede={version ? format(bydos.lede, { version }) : bydos.ledeFallback}
      action={
        <ArrowLink href={wikiPathOf(lang, 'enemies')}>{bydos.open}</ArrowLink>
      }
      panels={ENEMY_KINDS.map((kind, index) => {
        const entry = pages.wikiPage.enemies.entries[kind];
        return {
          id: kind,
          number: format(bydos.number, {
            number: index + 1,
            total: ENEMY_KINDS.length,
          }),
          name: entry.name,
          text: entry.text,
          sprite: (
            <Enemy
              kind={kind}
              scale={enemyScaleFor(kind, PANEL_SPRITE_PIXELS)}
              label={entry.name}
            />
          ),
        };
      })}
    />
  );
}
