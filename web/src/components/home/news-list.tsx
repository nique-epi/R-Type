import Link from 'next/link';
import { Triangle } from '@/components/primitives/icons/triangle';
import { cn } from '@/lib/utils';

export type NewsItem = {
  id: string;
  /** When it shipped, as the notes print it: "2 nov. 2026". */
  date: string;
  text: string;
  href: string;
};

/** The last changes of the game, one row each, a rule between two rows. */
export function NewsList({ items }: { items: readonly NewsItem[] }) {
  return (
    <ul data-slot="news-list" className="m-0 mt-10 list-none p-0">
      {items.map((item, index) => (
        <li key={item.id}>
          <Link
            href={item.href}
            className={cn(
              'row-link -mx-6 grid w-[calc(100%+48px)] grid-cols-[160px_minmax(0,1fr)_24px] items-center gap-6 px-6 py-[22px] text-left text-star-white max-md:grid-cols-1 max-md:gap-2',
              index > 0 && 'shadow-[inset_0_2px_0_rgb(38_48_94/60%)]',
            )}
          >
            <time className="small-caps text-pale-blue">{item.date}</time>
            <span className="text-lg leading-7">{item.text}</span>
            <Triangle className="text-hud-cyan max-md:hidden" />
          </Link>
        </li>
      ))}
    </ul>
  );
}
