import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { cn } from '@/lib/utils';

/**
 * The planet of PlanetImage.cpp, 157 × 85 pixels, drawn at ×6 across the
 * home page and at ×3 in the corner of a page hero.
 */
const planetVariants = cva(
  'pixelated pointer-events-none absolute bg-[url(/sky/planet.png)] bg-no-repeat drop-shadow-[0_0_40px_rgb(176_143_208/18%)]',
  {
    variants: {
      size: {
        large:
          'top-[14vh] left-[52vw] h-[510px] w-[942px] bg-size-[942px_510px] max-md:top-auto max-md:bottom-[6vh] max-md:left-[30vw] max-md:h-[255px] max-md:w-[471px] max-md:bg-size-[471px_255px]',
        small:
          'top-6 -right-20 h-[255px] w-[471px] bg-size-[471px_255px] max-md:opacity-40',
      },
    },
    defaultVariants: { size: 'small' },
  },
);

/** The ringed planet of the game sky; decoration, hidden from screen readers. */
export function Planet({
  size,
  className,
  ...props
}: React.ComponentProps<'div'> & VariantProps<typeof planetVariants>) {
  return (
    <div
      aria-hidden="true"
      data-slot="planet"
      className={cn(planetVariants({ size }), className)}
      {...props}
    />
  );
}
