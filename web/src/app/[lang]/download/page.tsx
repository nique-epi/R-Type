import type { Metadata } from 'next';
import { DownloadBlocks } from '@/components/blocks/download/download-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/download'>): Promise<Metadata> {
  return titleMetadata(params, 'downloadPage');
}

export default async function DownloadPage({
  params,
}: PageProps<'/[lang]/download'>) {
  return <DownloadBlocks lang={await readLocale(params)} />;
}
