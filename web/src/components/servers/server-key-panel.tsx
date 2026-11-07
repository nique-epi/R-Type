'use client';

import type * as React from 'react';
import { useId, useRef, useState } from 'react';
import { ScreenCorners } from '@/components/game/screen-corners';
import { ScrambleText } from '@/components/motion/scramble-text';
import { Button } from '@/components/primitives/button';
import { CodeBlock } from '@/components/primitives/code-block';
import { Panel, PanelTitle } from '@/components/primitives/panel';
import { useMotion } from '@/hooks/use-motion';
import { formatNodes } from '@/i18n/format-nodes';
import { gsap } from '@/lib/gsap';

export type ServerKeyPanelLabels = {
  title: string;
  /** "Le serveur {name} est créé. Copiez sa clé maintenant :". */
  created: string;
  /** "Puis collez-la dans le {file} du serveur :". */
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
  /** The line of that file that sets the key. */
  configLine: string;
  /** The other buttons: open the server, create another one. */
  actions?: React.ReactNode;
  labels: ServerKeyPanelLabels;
};

type CopyState = 'idle' | 'copied' | 'failed';

/**
 * The key of a server the master just created, shown once: the panel pops
 * in, the key appears scrambled and settles, and a button copies it.
 */
export function ServerKeyPanel({
  serverName,
  serverKey,
  configFile,
  configLine,
  actions,
  labels,
}: ServerKeyPanelProps) {
  const panel = useRef<HTMLElement>(null);
  const titleId = useId();
  const [copy, setCopy] = useState<CopyState>('idle');

  useMotion(() => {
    gsap.from(panel.current, {
      scale: 0.94,
      opacity: 0,
      duration: 0.5,
      ease: 'back.out(1.6)',
    });
  });

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
    <Panel
      ref={panel}
      aria-labelledby={titleId}
      className="relative"
      data-slot="server-key-panel"
    >
      <ScreenCorners className="border-hud-cyan" />
      <PanelTitle id={titleId}>{labels.title}</PanelTitle>
      <p className="m-0 mb-4">
        {formatNodes(labels.created, { name: strong(serverName) })}
      </p>
      <CodeBlock>
        <ScrambleText text={serverKey} />
      </CodeBlock>
      <p role="status" className="my-4 small-caps text-star-white">
        {status}
      </p>
      <p className="m-0 mb-2">
        {formatNodes(labels.pasteIn, { file: strong(configFile) })}
      </p>
      <CodeBlock>{configLine}</CodeBlock>
      <div className="mt-6 flex flex-wrap items-center gap-4">
        <Button variant="solid" onClick={copyKey}>
          {copy === 'copied' ? labels.copied : labels.copy}
        </Button>
        {actions}
      </div>
    </Panel>
  );
}
