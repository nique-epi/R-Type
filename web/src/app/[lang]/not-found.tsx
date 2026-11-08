'use client';

import { useParams } from 'next/navigation';
import { Button } from '@/components/primitives/actions/button';
import { Container } from '@/components/primitives/layout/container';
import { Heading } from '@/components/primitives/text/typography';
import { getDictionary } from '@/i18n/get-dictionary';
import { DEFAULT_LOCALE, isLocale } from '@/i18n/locales';
import { pathOf } from '@/lib/site-paths';

/** The page of a path that leads nowhere, in the language of the path. */
export default function NotFound() {
  const { lang } = useParams<{ lang: string }>();
  const locale = isLocale(lang) ? lang : DEFAULT_LOCALE;
  const { notFound } = getDictionary(locale).pages;

  return (
    <main className="py-30">
      <Container className="grid justify-items-start gap-6">
        <Heading as="h1" size="display">
          {notFound.title}
        </Heading>
        <p className="m-0 max-w-[62ch] text-lg leading-[30px]">
          {notFound.text}
        </p>
        <Button href={pathOf(locale, 'home')} variant="solid">
          {notFound.back}
        </Button>
      </Container>
    </main>
  );
}
