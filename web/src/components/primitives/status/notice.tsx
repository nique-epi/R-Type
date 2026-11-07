import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { cn } from '@/lib/utils';

const noticeVariants = cva('flex items-baseline gap-3 text-sm leading-[22px]', {
  variants: {
    tone: {
      warning:
        'rounded-2xl border-2 border-warning-orange px-4 py-3 text-warning-orange',
      info: 'text-pale-blue [&>[data-slot=notice-title]]:text-hud-cyan',
    },
  },
  defaultVariants: { tone: 'info' },
});

export type NoticeProps = React.ComponentProps<'div'> &
  VariantProps<typeof noticeVariants> & {
    /** The word that comes first, never the color alone: QUOTA ATTEINT. */
    title: React.ReactNode;
  };

/** A line that warns or informs, its title word first. */
export function Notice({
  className,
  tone,
  title,
  children,
  ...props
}: NoticeProps) {
  return (
    <div
      data-slot="notice"
      className={cn(noticeVariants({ tone }), className)}
      {...props}
    >
      <b
        data-slot="notice-title"
        className="whitespace-nowrap font-normal font-small text-base uppercase"
      >
        {title}
      </b>
      {children}
    </div>
  );
}
