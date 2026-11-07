import type { Metadata } from 'next';
import { FONT_VARIABLES } from '@/lib/fonts';
import '@/styles/globals.css';

export const metadata: Metadata = {
  title: 'R-Type',
  description: 'The game servers of R-Type and their master.',
};

export default function RootLayout({ children }: LayoutProps<'/'>) {
  return (
    <html lang="en" className={`${FONT_VARIABLES} h-full antialiased`}>
      <body className="flex min-h-full flex-col">{children}</body>
    </html>
  );
}
