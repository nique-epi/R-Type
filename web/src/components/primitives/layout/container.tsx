import type * as React from 'react';
import { cn } from '@/lib/utils';

/** The column of the site: 1200px at most, with its gutters. */
export function Container({
  className,
  ...props
}: React.ComponentProps<'div'>) {
  return (
    <div
      data-slot="container"
      className={cn(
        'mx-auto box-border w-full max-w-[1200px] px-8 max-md:px-4',
        className,
      )}
      {...props}
    />
  );
}
