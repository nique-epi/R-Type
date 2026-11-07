import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { cn } from '@/lib/utils';

const headingVariants = cva('m-0 font-pixel uppercase', {
  variants: {
    size: {
      /** The name of a Bydo on its panel. */
      giant:
        'text-[56px] text-star-white leading-16 max-md:text-[32px] max-md:leading-10',
      /** The title of a page or of a band of the home page. */
      display:
        'text-5xl text-star-white leading-16 max-md:text-[32px] max-md:leading-11',
      /** The title of a section of the home page. */
      title:
        'text-[32px] text-hud-cyan leading-12 max-md:text-2xl max-md:leading-9',
      /** The title of an article of the wiki. */
      article: 'text-[32px] text-star-white leading-10',
      /** A section of a page. */
      section: 'text-2xl text-hud-cyan leading-8',
      /** A card, a step, a row. */
      card: 'text-base text-star-white leading-6',
    },
  },
  defaultVariants: { size: 'section' },
});

type HeadingLevel = 'h1' | 'h2' | 'h3' | 'h4' | 'p';

export type HeadingProps = React.ComponentProps<'h2'> &
  VariantProps<typeof headingVariants> & {
    /** The element, chosen for the outline of the page, apart from the size. */
    as?: HeadingLevel;
  };

/** A heading in pixel capitals; its level and its size are chosen apart. */
export function Heading({
  as: Element = 'h2',
  size,
  className,
  ...props
}: HeadingProps) {
  return (
    <Element
      data-slot="heading"
      className={cn(headingVariants({ size }), className)}
      {...props}
    />
  );
}

/** The small line over a heading: "En ce moment", "Notes de version". */
export function Eyebrow({ className, ...props }: React.ComponentProps<'p'>) {
  return (
    <p
      data-slot="eyebrow"
      className={cn('m-0 mb-4 small-caps text-star-white', className)}
      {...props}
    />
  );
}

/** The paragraph that opens a section, in a larger reading size. */
export function Lede({ className, ...props }: React.ComponentProps<'p'>) {
  return (
    <p
      data-slot="lede"
      className={cn('m-0 mt-6 max-w-[62ch] text-lg leading-[30px]', className)}
      {...props}
    />
  );
}
