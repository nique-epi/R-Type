import type * as React from 'react';
import { cn } from '@/lib/utils';

/** The title of a panel, or of a short block of a page, in cyan capitals. */
function PanelTitle({ className, ...props }: React.ComponentProps<'h2'>) {
  return (
    <h2
      data-slot="panel-title"
      className={cn(
        'm-0 mb-4 border-edge-blue border-b-2 pb-3 font-pixel text-base text-hud-cyan uppercase leading-4',
        className,
      )}
      {...props}
    />
  );
}

export { PanelTitle };
