import Link from 'next/link';

export default function NotFound() {
  return (
    <main className="flex flex-1 flex-col items-center justify-center gap-4 p-6 text-center">
      <h1 className="font-semibold text-2xl tracking-tight">
        This page does not exist
      </h1>
      <Link
        href="/"
        className="rounded-lg bg-primary px-3 py-1.5 font-medium text-primary-foreground text-sm outline-none focus-visible:ring-3 focus-visible:ring-ring/50"
      >
        Back to the home page
      </Link>
    </main>
  );
}
