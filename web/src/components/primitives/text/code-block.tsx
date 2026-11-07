import type * as React from 'react';
import { cn } from '@/lib/utils';

/** A command or a key to copy, on one line that scrolls rather than wraps. */
export function CodeBlock({
  className,
  ...props
}: React.ComponentProps<'code'>) {
  return (
    <code
      data-slot="code-block"
      className={cn(
        'block overflow-x-auto whitespace-pre rounded-[14px] bg-ink/80 px-[18px] py-3.5 font-body font-medium text-sm text-star-white leading-[22px]',
        className,
      )}
      {...props}
    />
  );
}
