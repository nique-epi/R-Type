'use client';

import type * as React from 'react';
import { useState } from 'react';
import { Reveal } from '@/components/motion/reveal';
import { Heading } from '@/components/primitives/text/typography';
import { VideoCard } from './video-card';
import { VideoDialog } from './video-dialog';

/** A video of the page: its card, and the words around it. */
export type ShowcaseVideo = {
  id: string;
  label: string;
  title: string;
  /** The name of the card for a screen reader: "Regarder : à quatre pilotes". */
  aria: string;
  scene: React.ReactNode;
};

export type VideoShowcaseProps = {
  /** The video the page leads with, on a large card. */
  trailer: ShowcaseVideo & { playLabel: string };
  videos: readonly ShowcaseVideo[];
  /** The picture over the title in the dialog while the video is not out. */
  figure: React.ReactNode;
  /** The line under the title: "[Vidéo à venir]". */
  comingSoon: string;
  closeLabel: string;
};

/**
 * The trailer and the videos under it. A click opens the video in a dialog;
 * none is published yet, so the dialog holds a picture, the title and a line that says so.
 */
export function VideoShowcase({
  trailer,
  videos,
  figure,
  comingSoon,
  closeLabel,
}: VideoShowcaseProps) {
  const [opened, setOpened] = useState<string | null>(null);
  const title = [trailer, ...videos].find(video => video.id === opened)?.title;

  return (
    <>
      <Reveal className="grid gap-6">
        <VideoCard
          scene={trailer.scene}
          playLabel={trailer.playLabel}
          aria-label={trailer.aria}
          onClick={() => setOpened(trailer.id)}
        />
        <div className="grid grid-cols-3 gap-6 max-md:grid-cols-1">
          {videos.map(video => (
            <div key={video.id}>
              <VideoCard
                size="small"
                scene={video.scene}
                aria-label={video.aria}
                onClick={() => setOpened(video.id)}
              />
              <div className="mt-3.5 grid gap-2">
                <span className="small-caps text-star-white">
                  {video.label}
                </span>
                <Heading as="h3" size="card">
                  {video.title}
                </Heading>
              </div>
            </div>
          ))}
        </div>
      </Reveal>
      <VideoDialog
        open={opened !== null}
        onClose={() => setOpened(null)}
        title={title ?? ''}
        closeLabel={closeLabel}
      >
        <div className="grid justify-items-center gap-4 text-center">
          {figure}
          <p className="m-0 font-pixel text-base text-star-white uppercase leading-6">
            {title}
          </p>
          <p className="m-0 small-caps text-star-white">{comingSoon}</p>
        </div>
      </VideoDialog>
    </>
  );
}
