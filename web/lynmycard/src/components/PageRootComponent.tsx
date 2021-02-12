import React, { useEffect, PropsWithChildren, useState } from "react";

interface Props {
  className?: string;
  areaThreshold?: number;
  onTopAreaReached?: () => void | null;
  onBottomAreaReached?: () => void | null;
}

export const PageRootComponent: React.FunctionComponent<Props> = ({
  className,
  areaThreshold = 50,
  onTopAreaReached,
  onBottomAreaReached,
  children,
}: PropsWithChildren<Props>) => {
  const detectTopArea = (el: HTMLElement) => {
    const height = el.getBoundingClientRect().height - areaThreshold;
    return el.getBoundingClientRect().bottom >= height;
  };
  const detectBottomArea = (el: HTMLElement) => {
    const bottom = el.getBoundingClientRect().bottom - areaThreshold;
    return bottom <= window.innerHeight;
  };

  const [isTop, setIsTop] = useState(false);
  const [isBottom, setIsBottom] = useState(false);

  // 스크롤 영역 최상단/최하단 도달 이벤트
  useEffect(() => {
    const detect = () => {
      const scrollbox = document.getElementById("page-root");
      if (scrollbox !== null) {
        setIsTop(detectTopArea(scrollbox));
        setIsBottom(detectBottomArea(scrollbox));
      }
    };

    document.addEventListener("scroll", detect);
    return () => {
      document.removeEventListener("scroll", detect);
    };
  }, []);

  useEffect(() => {
    // if (process.browser) {
    //   // 스크롤 초기화
    //   window.scrollTo({
    //     left: 0,
    //     top: 0,
    //   });
    // }
    // 네이버 애널리틱스 API 트래킹
    // NaverAnalytics.trackEvent();
  }, []);

  useEffect(() => {
    if (isTop && onTopAreaReached) onTopAreaReached();
  }, [isTop]);
  useEffect(() => {
    if (isBottom && onBottomAreaReached) onBottomAreaReached();
  }, [isBottom]);
  return (
    <div className={className} id="page-root">
      {children}
    </div>
  );
};
