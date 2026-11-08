'use client';

import { useEffect, useState } from 'react';
import { Heading } from '@/components/primitives/text/typography';
import { format } from '@/i18n/format';
import { formatMegabytes } from '@/i18n/format-size';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import { detectSystem } from '@/lib/github/detect-system';
import { installStepsFor } from '@/lib/github/install-steps';
import type { ReleaseFile } from '@/lib/github/release';
import { SYSTEMS, type System } from '@/lib/github/systems';
import { DownloadArt } from './download-art';
import { DownloadPanel, type DownloadPanelLabels } from './download-panel';
import { InstallSteps } from './install-steps';

export type DownloadSectionProps = {
  locale: Locale;
  /** The installer of each system in the latest release; none when the release lacks it. */
  clients: Record<System, ReleaseFile | null>;
  /** "0.1.0", or nothing when the release could not be read. */
  version: string | null;
  copy: PageCopy['downloadPage'];
  labels: DownloadPanelLabels;
};

/** Starts the download of a file the way a click on its link would. */
function startDownload(file: ReleaseFile) {
  const link = document.createElement('a');
  link.href = file.url;
  link.download = file.name;
  link.click();
}

/**
 * The head of the download page: the installer of the system the reader runs
 * (detected once the page is in their browser, Linux until then), the button
 * that downloads it, and the three steps to install it. The browser does the
 * download, so the page cannot tell how far it is: it says it started.
 */
export function DownloadSection({
  locale,
  clients,
  version,
  copy,
  labels,
}: DownloadSectionProps) {
  const [detected, setDetected] = useState<System | null>(null);
  const [picked, setPicked] = useState<System | null>(null);
  const [started, setStarted] = useState<string | null>(null);
  const system = picked ?? detected ?? 'linux';

  useEffect(() => {
    setDetected(detectSystem(navigator.userAgent));
  }, []);

  const options = SYSTEMS.map(option => {
    const file = clients[option];
    const size = file
      ? format(copy.size, {
          kind: copy.kinds[option],
          size: formatMegabytes(file.size, locale),
        })
      : copy.unavailable;
    return {
      system: option,
      name: copy.systems[option],
      details: version && file ? `${size} · v${version}` : size,
      file: file?.name ?? '—',
    };
  });
  const file = clients[system];

  return (
    <>
      <div className="flex flex-wrap items-center gap-12">
        <div className="grid min-w-0 flex-[999_1_440px] gap-5">
          <DownloadPanel
            options={options}
            system={system}
            onSystemChange={chosen => {
              setPicked(chosen);
              setStarted(null);
            }}
            hint={system === detected ? copy.detected : undefined}
            buttonLabel={copy.button}
            busy={file === null}
            onDownload={() => {
              if (file) {
                startDownload(file);
                setStarted(file.name);
              }
            }}
            labels={labels}
          />
          <p role="status" className="m-0 min-h-4 small-caps text-star-white">
            {started ? format(copy.started, { file: started }) : null}
          </p>
        </div>
        <div className="max-md:hidden flex-[1_1_240px]">
          <DownloadArt />
        </div>
      </div>
      {file ? (
        <section aria-labelledby="install-title" className="grid gap-6">
          <Heading id="install-title">{copy.stepsTitle}</Heading>
          <InstallSteps
            steps={installStepsFor(system, copy.steps, file.name)}
            current={started ? 1 : 0}
          />
        </section>
      ) : null}
    </>
  );
}
