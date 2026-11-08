/** The part of StPageFlip (page-flip 2.0.7) the site uses; the package ships no types. */
declare module 'page-flip' {
  export type PageFlipSettings = {
    width: number;
    height: number;
    size?: 'fixed' | 'stretch';
    minWidth?: number;
    maxWidth?: number;
    minHeight?: number;
    maxHeight?: number;
    drawShadow?: boolean;
    flippingTime?: number;
    usePortrait?: boolean;
    maxShadowOpacity?: number;
    showCover?: boolean;
    mobileScrollSupport?: boolean;
    clickEventForward?: boolean;
    showPageCorners?: boolean;
    disableFlipByClick?: boolean;
  };

  export type FlipEvent = { data: number | string };

  export class PageFlip {
    constructor(parent: HTMLElement, settings: PageFlipSettings);
    loadFromHTML(items: HTMLElement[]): void;
    on(
      event: 'flip' | 'changeOrientation' | 'init',
      callback: (event: FlipEvent) => void,
    ): this;
    getOrientation(): 'portrait' | 'landscape';
    update(): void;
    flipNext(): void;
    flipPrev(): void;
    flip(page: number): void;
    getCurrentPageIndex(): number;
    getPageCount(): number;
    destroy(): void;
  }
}
