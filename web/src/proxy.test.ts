import { NextRequest } from 'next/server';
import { describe, expect, it } from 'vitest';

import { proxy } from './proxy';

function request(path: string, acceptLanguage?: string) {
  return new NextRequest(`http://localhost:3000${path}`, {
    headers: acceptLanguage ? { 'accept-language': acceptLanguage } : {},
  });
}

describe('proxy', () => {
  it('sends the home page to the language of the browser', () => {
    const response = proxy(request('/', 'en-GB,en;q=0.8'));

    expect(response.headers.get('location')).toBe('http://localhost:3000/en');
  });

  it('keeps the path and the query of a page asked with no language', () => {
    const response = proxy(request('/servers/7?tab=games', 'fr'));

    expect(response.headers.get('location')).toBe(
      'http://localhost:3000/fr/servers/7?tab=games',
    );
  });

  it('sends a browser that names no language to French', () => {
    expect(proxy(request('/download')).headers.get('location')).toBe(
      'http://localhost:3000/fr/download',
    );
  });

  it('tells the caches the answer depends on the language', () => {
    expect(proxy(request('/', 'en')).headers.get('vary')).toBe(
      'Accept-Language',
    );
  });

  it('lets a page that has its language through', () => {
    const response = proxy(request('/en/servers', 'fr'));

    expect(response.headers.get('location')).toBeNull();
    expect(response.headers.get('x-middleware-next')).toBe('1');
  });

  it('does not take a page whose name only starts like a language for one', () => {
    expect(proxy(request('/english', 'en')).headers.get('location')).toBe(
      'http://localhost:3000/en/english',
    );
  });
});
