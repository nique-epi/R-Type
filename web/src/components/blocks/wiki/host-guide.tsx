import { Reveal } from '@/components/motion/reveal';
import { Button } from '@/components/primitives/actions/button';
import { CodeBlock } from '@/components/primitives/text/code-block';
import { Heading } from '@/components/primitives/text/typography';
import { formatNodes } from '@/i18n/format-nodes';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { pathOf } from '@/lib/site-paths';

/** The lines of `server.cfg` that list a server on a master, as `server.cfg.example` writes them. */
const CONFIG_LINES = [
  'set master-url "<the address of the master>"',
  'set master-key "rtk_..."',
];

/** The command that starts the server once its key is in place. */
const START_COMMAND = './r-type_server';

/** The three steps that put a server on the map, and the button that makes the key. */
export function HostGuide({ lang }: { lang: Locale }) {
  const { pages } = getDictionary(lang);
  const { host } = pages.wikiPage;
  const { configFile } = pages.hostPage;

  return (
    <Reveal className="grid gap-8">
      <Heading as="h2" size="article">
        {host.title}
      </Heading>
      <ol className="m-0 grid gap-4 pl-6">
        <li>{host.createKey}</li>
        <li>
          {formatNodes(host.pasteKey, {
            file: <strong className="text-star-white">{configFile}</strong>,
          })}
          <CodeBlock className="mt-2 block whitespace-pre">
            {CONFIG_LINES.join('\n')}
          </CodeBlock>
        </li>
        <li>
          {host.launch}
          <CodeBlock className="mt-2 block">{START_COMMAND}</CodeBlock>
        </li>
      </ol>
      <div>
        <Button href={pathOf(lang, 'hostServer')} variant="solid">
          {host.button}
        </Button>
      </div>
    </Reveal>
  );
}
