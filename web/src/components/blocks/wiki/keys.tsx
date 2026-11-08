import { Reveal } from '@/components/motion/reveal';
import { Heading, Lede } from '@/components/primitives/text/typography';
import { KeyBindingsTable } from '@/components/wiki/key-bindings-table';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The commands of the game: the keys on caps, and what they do. */
export function Keys({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const { keys } = dictionary.pages.wikiPage;

  return (
    <Reveal className="grid gap-8">
      <div>
        <Heading as="h2" size="article">
          {keys.title}
        </Heading>
        <Lede>{keys.lede}</Lede>
      </div>
      <KeyBindingsTable
        bindings={keys.bindings}
        labels={dictionary.keyBindings}
      />
    </Reveal>
  );
}
