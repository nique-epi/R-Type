import type * as React from 'react';
import { cn } from '@/lib/utils';

/** The content of an inner page under its hero: a column of sections. */
export function PageMain({
  className,
  ...props
}: React.ComponentProps<'main'>) {
  return (
    <main
      data-slot="page-main"
      className={cn(
        'mx-auto box-border grid w-full max-w-[1200px] gap-14 p-8 max-md:gap-8 max-md:px-4 max-md:py-6',
        className,
      )}
      {...props}
    />
  );
}
