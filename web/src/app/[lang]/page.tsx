import { Container } from '@/components/primitives/layout/container';
import { Heading } from '@/components/primitives/text/typography';

export default function HomePage() {
  return (
    <main className="py-30">
      <Container>
        <Heading as="h1" size="display">
          R-Type
        </Heading>
      </Container>
    </main>
  );
}
