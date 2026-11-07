import Link from 'next/link';
import type { NavItem } from '@/components/layout/nav-item';
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
                  'group/side relative flex w-full items-center py-2.5 pl-[22px] font-pixel text-xs uppercase leading-4 no-underline transition-colors duration-200 hover:text-star-white max-md:w-auto',
                  shown ? 'text-hud-cyan' : 'text-pale-blue',
                )}
              >
                <span
                  aria-hidden="true"
                  className={cn(
                    'absolute top-1/2 left-0 -mt-1 size-2 bg-hud-cyan transition-transform duration-200',
                    shown ? 'scale-100' : 'scale-0',
                  )}
                />
                {item.label}
              </Link>
            </li>
          );
        })}
      </ul>
    </nav>
  );
}
