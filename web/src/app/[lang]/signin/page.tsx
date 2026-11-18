import type { Metadata } from 'next';
import { SignInBlocks } from '@/components/blocks/sign-in/sign-in-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/signin'>): Promise<Metadata> {
  return titleMetadata(params, 'signInPage');
}

export default async function SignInPage({
  params,
  searchParams,
}: PageProps<'/[lang]/signin'>) {
  const { next } = await searchParams;
  return (
    <SignInBlocks
      lang={await readLocale(params)}
      next={typeof next === 'string' ? next : undefined}
    />
  );
}
