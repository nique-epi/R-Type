import type * as React from 'react';
import { Triangle } from '@/components/primitives/icons/triangle';
import { cn } from '@/lib/utils';

export type VideoCardProps = Omit<
  React.ComponentProps<'button'>,
  'children'
> & {
  /** The picture of the video: a scene of sprites on the sky. */
  scene: React.ReactNode;
  /** The word beside the play disc of the large card: "Regarder". */
  playLabel?: React.ReactNode;
  size?: 'large' | 'small';
  'aria-label': string;
};

/**
 * The thumbnail of a video: a 16:9 scene that zooms in slightly on hover,
 * with a white play disc in its corner. A click opens the video.
 */
export function VideoCard({
  scene,
  playLabel,
  size = 'large',
  className,
  type = 'button',
  ...props
}: VideoCardProps) {
  const large = size === 'large';
  return (
    <button
      type={type}
      data-slot="video-card"
      className={cn(
        'group/video relative block aspect-video w-full cursor-pointer overflow-hidden rounded-3xl bg-ink p-0 text-left text-star-white focus-visible:outline-offset-3',
        className,
      )}
      {...props}
    >
      <span className="absolute inset-0 block transition-transform duration-800 ease-[cubic-bezier(.2,.7,.2,1)] group-hover/video:scale-105">
        {scene}
      </span>
      <span
        className={cn(
          'absolute inline-flex items-center gap-3.5',
          large ? 'bottom-6 left-6' : 'bottom-4 left-4',
        )}
      >
        <span
          className={cn(
            'grid place-items-center rounded-full bg-star-white text-ink',
            large ? 'size-14' : 'size-10',
          )}
        >
          <Triangle className="ml-1" />
        </span>
        {large && playLabel ? (
          <span className="font-pixel text-base uppercase leading-6">
            {playLabel}
          </span>
        ) : null}
      </span>
    </button>
  );
}
