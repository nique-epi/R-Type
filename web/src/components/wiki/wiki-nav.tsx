import Link from 'next/link';
import type { NavItem } from '@/components/layout/nav-item';
import {
  markedRowVariants,
  SideMarker,
} from '@/components/primitives/icons/side-marker';
import { cn } from '@/lib/utils';

export type WikiNavProps = {
  items: readonly NavItem[];
  /** The href of the article shown. */
  current: string;
  'aria-label': string;
};

/** The articles of the wiki down the side of the page, a cyan square before the one shown. */
export function WikiNav({ items, current, 'aria-label': label }: WikiNavProps) {
  return (
    <nav
      aria-label={label}
      data-slot="wiki-nav"
      className="max-w-60 flex-[1_1_200px] max-md:max-w-none"
    >
      <ul className="m-0 flex list-none flex-col gap-2 p-0 max-md:flex-row max-md:flex-wrap">
        {items.map(item => {
          const shown = item.href === current;
          return (
            <li key={item.href}>
              <Link
                href={item.href}
                aria-current={shown ? 'page' : undefined}
                className={cn(
                  markedRowVariants({ marked: shown }),
                  'font-pixel text-xs uppercase leading-4 no-underline max-md:w-auto',
                )}
              >
                <SideMarker shown={shown} />
                {item.label}
              </Link>
            </li>
          );
        })}
      </ul>
    </nav>
  );
}
