import type { Metadata } from 'next';
import { LinkBlocks } from '@/components/blocks/link/link-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/link'>): Promise<Metadata> {
  return titleMetadata(params, 'linkPage');
}

export default async function LinkPage({
  params,
  searchParams,
}: PageProps<'/[lang]/link'>) {
  const { code } = await searchParams;
  return (
    <LinkBlocks
      lang={await readLocale(params)}
      code={typeof code === 'string' ? code : undefined}
    />
  );
}
