import { NewsList } from '@/components/home/news-list';
import { SectionHeader } from '@/components/home/section-header';
import { Reveal } from '@/components/motion/reveal';
import { ArrowLink } from '@/components/primitives/actions/arrow-link';
import { Container } from '@/components/primitives/layout/container';
import { Section } from '@/components/primitives/layout/section';
import { format } from '@/i18n/format';
import { formatDay } from '@/i18n/format-date';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import type { LatestRelease } from '@/lib/github/release';
import { pathOf } from '@/lib/site-paths';

/** The newest entries of the notes of the latest release; nothing when there is no release. */
export function News({
  lang,
  release,
}: {
  lang: Locale;
  release: LatestRelease | null;
}) {
  const { news } = getDictionary(lang).pages.home;

  if (!release || release.notes.length === 0) {
    return null;
  }

  const date = formatDay(release.publishedAt, lang, 'short');

  return (
    <Section aria-labelledby="news-title" className="pt-0">
      <Container>
        <Reveal>
          <SectionHeader
            eyebrow={news.eyebrow}
            title={news.title}
            titleId="news-title"
            action={
              <ArrowLink href={pathOf(lang, 'download')}>
                {format(news.download, { version: release.version })}
              </ArrowLink>
            }
          />
        </Reveal>
        <Reveal targets="li" start="top 92%">
          <NewsList
            items={release.notes.map(text => ({
              id: text,
              date,
              text,
              href: pathOf(lang, 'download'),
            }))}
          />
        </Reveal>
      </Container>
    </Section>
  );
}
