import type * as React from 'react';
import { Reveal, type RevealProps } from '@/components/motion/reveal';
import { PageMain } from '@/components/primitives/layout/page-main';

/**
 * The content of an inner page under its hero. Its blocks rise into view as
 * they scroll in; `targets` picks the elements that rise when they are not
 * its direct children.
 */
export function PageBody({
  children,
  ...reveal
}: { children: React.ReactNode } & Pick<RevealProps, 'targets' | 'from'>) {
  return (
    <PageMain>
      <Reveal className="grid gap-14 max-md:gap-8" from={{ y: 48 }} {...reveal}>
        {children}
      </Reveal>
    </PageMain>
  );
}
