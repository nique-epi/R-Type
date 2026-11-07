'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { PlayButton } from '@/components/game/play-button';
import { CodeBlock } from '@/components/primitives/code-block';
import { PixelIcon } from '@/components/primitives/pixel-icon';
import { ProgressBar } from '@/components/primitives/progress-bar';
import { Tabs } from '@/components/primitives/tabs';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

/** A package of the client for one system. */
export type DownloadOption<System extends string> = {
  system: System;
  /** "Linux". */
  name: string;
  /** "AppImage · x86_64 · 52 Mo · v0.1.0". */
  details: string;
  file: string;
};

export type DownloadPanelLabels = {
  systems: string;
  progress: string;
};

export type DownloadPanelProps<System extends string> = {
  options: readonly DownloadOption<System>[];
  system: System;
  onSystemChange: (system: System) => void;
  /** The line over the name, such as "Votre système" when it was detected. */
  hint?: React.ReactNode;
  buttonLabel: React.ReactNode;
  onDownload: () => void;
  busy?: boolean;
  /** From 0 to 100 while a download runs; nothing is shown without it. */
  progress?: number;
  /** The line under the bar: "37 / 52 Mo", or "Téléchargé". */
  progressText?: React.ReactNode;
  labels: DownloadPanelLabels;
};

/**
 * Picks the package of a system and downloads it: a tab per system, the
 * name and the file of the package, the download button, and a progress
 * bar while it runs. The head steps in again each time the system changes.
 */
export function DownloadPanel<System extends string>({
  options,
  system,
  onSystemChange,
  hint,
  buttonLabel,
  onDownload,
  busy = false,
  progress,
  progressText,
  labels,
}: DownloadPanelProps<System>) {
  const head = useRef<HTMLDivElement>(null);
  const chosen = options.find(option => option.system === system) ?? options[0];

  useMotion(
    () => {
      gsap.from(head.current?.children ?? [], {
        x: -16,
        opacity: 0,
        duration: 0.25,
        ease: 'steps(4)',
        stagger: 0.05,
      });
    },
    { dependencies: [system] },
  );

  if (chosen === undefined) {
    return null;
  }
  return (
    <div data-slot="download-panel" className="grid min-w-0 gap-5">
      <Tabs
        aria-label={labels.systems}
        items={options.map(option => ({
          value: option.system,
          label: option.name,
        }))}
        value={chosen.system}
        onValueChange={onSystemChange}
      />
      <div ref={head} className="grid gap-3">
        <p className="m-0 min-h-4 small-caps text-star-white">{hint}</p>
        <h2 className="m-0 font-pixel text-[32px] text-hud-cyan uppercase leading-10">
          {chosen.name}
        </h2>
        <p className="m-0 small-caps text-pale-blue">{chosen.details}</p>
        <CodeBlock>{chosen.file}</CodeBlock>
      </div>
      <div className="flex flex-wrap items-center gap-6">
        <PlayButton
          size="lg"
          icon={<PixelIcon name="download" size={28} />}
          disabled={busy}
          onLaunch={onDownload}
        >
          {buttonLabel}
        </PlayButton>
      </div>
      {progress === undefined ? null : (
        <div className="grid gap-2">
          <ProgressBar value={progress} aria-label={labels.progress} />
          <p role="status" className="m-0 small-caps text-pale-blue">
            {progressText}
          </p>
        </div>
      )}
    </div>
  );
}
