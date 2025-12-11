import { onUnmounted, ref } from "vue";
import type { UseCountdownOptions } from "../types";

export default function useCountDown({
  onComplete,
  onTick,
}: UseCountdownOptions)
{
  const secondsLeft = ref(0);
  // const secondsLeftComputed = computed(() => secondsLeft.value);

  let ticker: ReturnType<typeof setInterval> | null = null;

  const clearTicker = () =>
  {
    if (ticker)
    {
      clearInterval(ticker);
      ticker = null;
    }
  };

  onUnmounted(() =>
  {
    clearTicker();
  });

  const startTimer = (seconds: number) =>
  {
    if (ticker)
    {
      clearInterval(ticker);
    }
    secondsLeft.value = seconds;

    ticker = setInterval(() =>
    {
      if (secondsLeft.value <= 1)
      {
        secondsLeft.value = 0;
        clearInterval(ticker!);
        ticker = null;
        onTick?.();
        onComplete?.();
      } else
      {
        secondsLeft.value -= 1;
        onTick?.();
      }
    }, 1000);
  };

  return {
    secondsLeft,
    startTimer,
  };
}
