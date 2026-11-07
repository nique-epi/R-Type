'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { IconButton } from '@/components/primitives/actions/icon-button';
import { PixelIcon } from '@/components/primitives/icons/pixel-icon';
import { Dialog } from '@/components/primitives/surfaces/dialog';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

export type VideoDialogProps = {
  open: boolean;
  onClose: () => void;
  title: string;
  closeLabel: string;
  /** The player, or a placeholder while the video is not out. */
  children: React.ReactNode;
};

/** A video over the darkened page, in a 16:9 screen that grows in as it opens. */
export function VideoDialog({
  open,
  onClose,
  title,
  closeLabel,
  children,
}: VideoDialogProps) {
  const screen = useRef<HTMLDivElement>(null);

  useMotion(
    () => {
      if (open) {
        gsap.from(screen.current, {
          scale: 0.9,
          opacity: 0,
          duration: 0.5,
          ease: 'power3.out',
        });
      }
    },
    { dependencies: [open] },
  );

  return (
    <Dialog
      open={open}
      onClose={onClose}
      aria-label={title}
      className="inset-0 size-full place-items-center p-6 backdrop:bg-sky/92 open:grid"
    >
      <div
        ref={screen}
        className="relative grid aspect-video w-[min(960px,100%)] place-items-center overflow-hidden rounded-xl bg-ink text-center"
      >
        {children}
        <IconButton
          aria-label={closeLabel}
          onClick={onClose}
          className="absolute top-2 right-2 bg-star-white/8"
        >
          <PixelIcon name="close" />
        </IconButton>
      </div>
    </Dialog>
  );
}
