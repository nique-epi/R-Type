import { DownloadSection } from '@/components/download/download-section';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import { getLatestRelease } from '@/hooks/api';
import { format } from '@/i18n/format';
import { formatDay } from '@/i18n/format-date';
import { formatMegabytes } from '@/i18n/format-size';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { releasesPageUrl } from '@/lib/github/latest-release';

/** The download page under its hero: the installer of the reader's system, its steps, the notes and the other files of the latest release. */
export async function DownloadBlocks({ lang }: { lang: Locale }) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.downloadPage;
  const release = await getLatestRelease();
  const releasesUrl = releasesPageUrl();

  return (
    <>
      <PageHero
        eyebrow={
          release
            ? format(copy.eyebrow, {
                version: release.version,
                date: formatDay(release.publishedAt, lang),
              })
            : copy.eyebrowFallback
        }
        title={copy.title}
        lede={copy.lede}
      />
      <PageBody>
        <DownloadSection
          locale={lang}
          clients={
            release?.clients ?? { linux: null, windows: null, macos: null }
          }
          version={release?.version ?? null}
          copy={copy}
          labels={dictionary.download}
        />
        <div className="grid grid-cols-2 gap-8 max-md:grid-cols-1">
          <section>
            <PanelTitle>{copy.requirementsTitle}</PanelTitle>
            <ul className="m-0 pl-5">
              {copy.requirements.map(line => (
                <li key={line}>{line}</li>
              ))}
            </ul>
          </section>
          {release && release.notes.length > 0 ? (
            <section>
              <PanelTitle>{copy.notesTitle}</PanelTitle>
              <p className="m-0 mb-2 small-caps text-star-white">
                v{release.version}
              </p>
              <ul className="m-0 pl-5">
                {release.notes.map(note => (
                  <li key={note}>{note}</li>
                ))}
              </ul>
            </section>
          ) : null}
        </div>
        <section>
          <PanelTitle>{copy.othersTitle}</PanelTitle>
          <ul className="m-0 grid list-none gap-2 p-0">
            {release?.others.map(file => (
              <li key={file.name}>
                <a
                  href={file.url}
                  className="text-hud-cyan hover:text-star-white"
                >
                  {file.name}
                </a>{' '}
                <span className="small-caps">
                  {format(copy.otherSize, {
                    size: formatMegabytes(file.size, lang),
                  })}
                </span>
              </li>
            ))}
            <li>
              <a
                href={releasesUrl}
                target="_blank"
                rel="noopener noreferrer"
                className="text-hud-cyan hover:text-star-white"
              >
                {copy.allReleases}
              </a>
            </li>
          </ul>
        </section>
      </PageBody>
    </>
  );
}
