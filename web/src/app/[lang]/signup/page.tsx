import type { Metadata } from 'next';
import { SignUpBlocks } from '@/components/blocks/sign-up/sign-up-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/signup'>): Promise<Metadata> {
  return titleMetadata(params, 'signUpPage');
}

export default async function SignUpPage({
  params,
}: PageProps<'/[lang]/signup'>) {
  return <SignUpBlocks lang={await readLocale(params)} />;
}
