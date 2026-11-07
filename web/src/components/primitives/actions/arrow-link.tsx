import { Triangle } from '@/components/primitives/icons/triangle';
import { cn } from '@/lib/utils';
import { Action, type ActionProps } from './action';

/**
 * A word in pixel capitals followed by an arrow that steps forward on hover:
 * "Carte du secteur ▸". A link when given an `href`, a button otherwise.
 */
export function ArrowLink({ className, children, ...props }: ActionProps) {
  return (
    <Action
      data-slot="arrow-link"
      className={cn(
        'group/arrow inline-flex cursor-pointer items-center gap-3 font-pixel text-hud-cyan text-xs uppercase leading-4 no-underline',
        className,
      )}
      {...props}
    >
      {children}
      <Triangle className="transition-transform duration-250 group-hover/arrow:translate-x-1.5 group-focus-visible/arrow:translate-x-1.5" />
    </Action>
  );
}
