import { useEffect, useState } from "react";

type UseCountdownOptions = {
  onComplete?: () => void; // Called when countdown reaches 0
  onTick?: () => void; // Called every tick
};

export default function useCountDown({
  onComplete,
  onTick,
}: UseCountdownOptions) {
  const [secondsLeft, setSecondsLeft] = useState(0);
  useEffect(() => {
    if (secondsLeft <= 0) {
      onComplete && onComplete();
      return;
    }

    onTick && onTick();

    // delay one second
    const timeout = setTimeout(function () {
      setSecondsLeft(secondsLeft - 1);
      console.log("secondsLeft: " + secondsLeft);
    }, 1000);

    // clean up
    return () => clearTimeout(timeout);
  }, [secondsLeft]);

  const startTimer = (seconds: number) => {
    setSecondsLeft(seconds);
  };

  return {
    secondsLeft,
    startTimer,
  };
}
