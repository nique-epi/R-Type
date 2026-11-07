import { Heading } from '@/components/primitives/text/typography';
import { GamesTable } from '@/components/servers/games-table';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import type { MapServer } from '@/lib/rtype/servers';
import { gameRowsOf } from './game-rows';

/** The games of a server in a table, or a line that says it has none. */
export function ServerGames({
  lang,
  server,
}: {
  lang: Locale;
  server: MapServer;
}) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.serverPage;

  return (
    <section className="grid gap-4">
      <Heading size="section">{copy.gamesTitle}</Heading>
      {server.games.length > 0 ? (
        <GamesTable
          rows={gameRowsOf(server, copy, dictionary.gameState)}
          labels={dictionary.gamesTable}
        />
      ) : (
        <p className="m-0">{copy.noGames}</p>
      )}
    </section>
  );
}
