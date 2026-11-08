import { notFound } from 'next/navigation';

/**
 * Every path under a language that no page claims: it ends in the not-found
 * page of the language, drawn inside the layout of the site. Without it an
 * unknown path would have no layout, since the layout of the site is the
 * root one.
 */
export default function UnknownPage() {
  notFound();
}
