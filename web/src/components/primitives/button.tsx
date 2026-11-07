import { cva, type VariantProps } from 'class-variance-authority';
import { cn } from '@/lib/utils';
import { Action, type ActionProps } from './action';

/**
 * The bar of the game menus, rounded for the site: an outlined pill, or a
 * filled one for the action a page leads to. The classes are exported so a
 * link outside these primitives can look like a button.
 */
export const buttonVariants = cva(
  'inline-flex shrink-0 cursor-pointer items-center justify-center gap-3 rounded-full border-2 font-pixel uppercase no-underline transition-colors duration-250 disabled:cursor-not-allowed disabled:border-deep-blue disabled:bg-sky disabled:text-blue',
  {
    variants: {
      variant: {
        outline:
          'border-edge-blue bg-transparent text-star-white enabled:hover:bg-edge-blue enabled:hover:text-ink focus-visible:bg-edge-blue focus-visible:text-ink',
        solid:
          'border-star-white bg-star-white text-ink enabled:hover:border-hud-cyan enabled:hover:bg-hud-cyan focus-visible:border-hud-cyan focus-visible:bg-hud-cyan',
      },
      size: {
        md: 'min-h-12 px-[22px] text-sm leading-[14px] max-md:min-h-10 max-md:px-3.5 max-md:text-xs',
        sm: 'min-h-9 px-4 text-xs leading-3',
      },
    },
    defaultVariants: { variant: 'outline', size: 'md' },
  },
);

export type ButtonProps = ActionProps & VariantProps<typeof buttonVariants>;

/** A button, or a link when given an `href`. */
export function Button({ className, variant, size, ...props }: ButtonProps) {
  return (
    <Action
      data-slot="button"
      className={cn(buttonVariants({ variant, size }), className)}
      {...props}
    />
  );
}
