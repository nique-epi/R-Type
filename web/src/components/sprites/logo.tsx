import Image from 'next/image';
import { cn } from '@/lib/utils';

/** The size of assets/sprites/logo.png, the logo of the title screen. */
const LOGO_WIDTH = 210;
const LOGO_HEIGHT = 42;

export type LogoProps = {
  /** Screen pixels per pixel of the logo: 2/3 in the header, 3 on the home page. */
  scale?: number;
  alt: string;
  priority?: boolean;
  className?: string;
};

/** The R-Type logo of the game, drawn at its scale without blurring. */
export function Logo({ scale = 2 / 3, alt, priority, className }: LogoProps) {
  return (
    <Image
      src="/sprites/logo.png"
      alt={alt}
      width={Math.round(LOGO_WIDTH * scale)}
      height={Math.round(LOGO_HEIGHT * scale)}
      priority={priority}
      unoptimized
      className={cn('pixelated block h-auto', className)}
    />
  );
}
