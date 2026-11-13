'use client';

import type * as React from 'react';
import { useId, useState } from 'react';
import { ScreenCorners } from '@/components/game/screen-corners';
import { Button } from '@/components/primitives/actions/button';
import { Notice } from '@/components/primitives/status/notice';
import { PanelTitle } from '@/components/primitives/surfaces/panel';
import { CodeBlock } from '@/components/primitives/text/code-block';
import { formatNodes } from '@/i18n/format-nodes';

export type ServerKeyPanelLabels = {
  title: string;
  /** "Le serveur {name} est créé. Voici sa clé :". */
  created: string;
  shownOnceTitle: string;
  shownOnce: string;
  /** "Collez ces lignes dans le {file} du serveur :". */
  pasteIn: string;
  copy: string;
  copied: string;
  /** The line under the key, before a copy, after it, and when the browser refused it. */
  notCopiedYet: string;
  copiedLine: string;
  copyFailed: string;
};

export type ServerKeyPanelProps = {
  serverName: string;
  serverKey: string;
  /** The file the key goes in: "server.cfg". */
  configFile: string;
  /** The lines of that file that list the server on the master. */
  configLines: readonly string[];
  /** The other buttons: read the guide, create another server. */
  actions?: React.ReactNode;
  labels: ServerKeyPanelLabels;
};

type CopyState = 'idle' | 'copied' | 'failed';

/**
 * The key of a server the master just created, shown this once: the key, a
 * button that copies it, and the lines of `server.cfg` that hold it. The
 * panel says the key will not show again, since nothing keeps it.
 */
export function ServerKeyPanel({
  serverName,
  serverKey,
  configFile,
  configLines,
  actions,
  labels,
}: ServerKeyPanelProps) {
  const titleId = useId();
  const [copy, setCopy] = useState<CopyState>('idle');

  async function copyKey() {
    try {
      await navigator.clipboard.writeText(serverKey);
      setCopy('copied');
    } catch {
      setCopy('failed');
    }
  }

  const status = {
    idle: labels.notCopiedYet,
    copied: labels.copiedLine,
    failed: labels.copyFailed,
  }[copy];
  const strong = (text: string) => (
    <strong className="font-normal text-star-white">{text}</strong>
  );

  return (
    <section
      aria-labelledby={titleId}
      className="relative grid gap-4 p-6"
      data-slot="server-key-panel"
    >
      <ScreenCorners className="border-hud-cyan" />
      <PanelTitle id={titleId} className="m-0">
        {labels.title}
      </PanelTitle>
      <p className="m-0">
        {formatNodes(labels.created, { name: strong(serverName) })}
      </p>
      <CodeBlock data-slot="server-key">{serverKey}</CodeBlock>
      <p role="status" className="m-0 small-caps text-star-white">
        {status}
      </p>
      <Notice tone="warning" title={labels.shownOnceTitle}>
        {labels.shownOnce}
      </Notice>
      <p className="m-0">
        {formatNodes(labels.pasteIn, { file: strong(configFile) })}
      </p>
      <CodeBlock data-slot="server-config">{configLines.join('\n')}</CodeBlock>
      <div className="flex flex-wrap items-center gap-4">
        <Button variant="solid" onClick={copyKey}>
          {copy === 'copied' ? labels.copied : labels.copy}
        </Button>
        {actions}
      </div>
    </section>
  );
}
