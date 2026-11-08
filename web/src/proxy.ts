import { type NextRequest, NextResponse } from 'next/server';

import { isLocale } from '@/i18n/locales';
import { negotiateLocale } from '@/i18n/negotiate-locale';

/**
 * Sends a request for a path with no language (`/servers`) to the same path
 * in the language its browser prefers (`/en/servers`), French when it names
 * none the site speaks. The answer depends on `Accept-Language`, so it says so.
 */
export function proxy(request: NextRequest) {
  const { pathname } = request.nextUrl;

  if (isLocale(pathname.split('/')[1] ?? '')) {
    return NextResponse.next();
  }

  const locale = negotiateLocale(request.headers.get('accept-language'));
  const target = request.nextUrl.clone();
  target.pathname = `/${locale}${pathname === '/' ? '' : pathname}`;

  const response = NextResponse.redirect(target);
  response.headers.set('Vary', 'Accept-Language');
  return response;
}

export const config = {
  /** Every page; not the files of Next.js (`/_next`) nor of `public/` (a path with a dot). */
  matcher: ['/((?!_next|.*\\..*).*)'],
};
