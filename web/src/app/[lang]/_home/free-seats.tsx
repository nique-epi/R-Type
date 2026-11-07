import { SectionHeader } from '@/components/home/section-header';
import { ServerRow } from '@/components/home/server-row';
import { Reveal } from '@/components/motion/reveal';
import { ArrowLink } from '@/components/primitives/actions/arrow-link';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { freeSeatsOf, pilotsOf } from '@/components/servers/sector-map/rules';
import { getServerList } from '@/hooks/api';
import { format, selectCountTemplate } from '@/i18n/format';
import { formatNodes } from '@/i18n/format-nodes';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { withoutAccents } from '@/lib/rtype/pixel-text';
import { pathOf, serverPathOf } from '@/lib/site-paths';

/** The servers the band lists. */
const SHOWN_SERVERS = 3;

/** The servers with seats open to a pilot, as rows that lead to their pages. */
export async function FreeSeats({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const { freeSeats } = dictionary.pages.home;
  const { servers, source } = await getServerList();
  const rows = servers
    .filter(server => freeSeatsOf(server) > 0)
    .slice(0, SHOWN_SERVERS);

  return (
    <Section aria-labelledby="free-seats-title">
      <Container>
        <Reveal>
          <SectionHeader
            eyebrow={freeSeats.eyebrow}
            title={freeSeats.title}
            titleId="free-seats-title"
            action={
              <ArrowLink href={pathOf(lang, 'servers')}>
                {freeSeats.map}
              </ArrowLink>
            }
            className="mb-6"
          />
        </Reveal>
        {rows.length > 0 ? (
          <Reveal className="m-0 grid list-none p-0">
            {rows.map(server => (
              <ServerRow
                key={server.id}
                href={serverPathOf(lang, server.id)}
                name={withoutAccents(server.name)}
                status={server.status}
                statusLabel={dictionary.status[server.status]}
                aria-label={format(freeSeats.open, { name: server.name })}
                seats={
                  <>
                    {formatNodes(
                      selectCountTemplate(
                        freeSeatsOf(server),
                        freeSeats.free,
                        lang,
                      ),
                      { count: <b>{freeSeatsOf(server)}</b> },
                    )}
                    {' · '}
                    {format(
                      selectCountTemplate(
                        pilotsOf(server),
                        freeSeats.pilots,
                        lang,
                      ),
                      { count: pilotsOf(server) },
                    )}
                  </>
                }
              />
            ))}
          </Reveal>
        ) : (
          <p className="m-0">{freeSeats.none}</p>
        )}
        {source === 'sample' ? (
          <p className="mt-6 mb-0 small-caps">
            {dictionary.pages.site.sampleServers}
          </p>
        ) : null}
      </Container>
    </Section>
  );
}
